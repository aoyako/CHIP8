#include "processor.hpp"

#include <algorithm>

Processor::Processor() {
  this->initialize_memory();
  this->initialize_font();
}

void Processor::load_program(std::vector<uint8_t> program_data) {
  for (int i = 0; i < std::min(program_data.size(),
                               std::size_t(Processor::MEMORY_SIZE -
                                           Processor::PROGRAM_DATA_START));
       ++i) {
    this->memory[i] = program_data[i];
  }
}

auto Processor::run(Keyboard key) -> Code {
  auto command = (uint16_t(this->memory[this->program_counter]) << 8) +
                 (uint16_t(this->memory[this->program_counter + 1]) << 8);

  if (command == 0x00E0) {  // 00E0
    this->clean_display();
    this->program_counter += 2;
    return Code::DRW;
  } else if (command == 0x00EE) {  // 00EE
    this->program_counter = this->stack[this->stack_pointer];
    this->stack_pointer--;
  } else if (command & 0xF000 == 0x1000) {  // 1nnn
    this->program_counter = command & 0x0FFF;
  } else if (command & 0xF000 == 0x2000) {  // 2nnn
    this->stack_pointer++;
    this->stack[this->stack_pointer] = this->program_counter;
    this->program_counter = command & 0x0FFF;
  } else if (command & 0xF000 == 0x3000) {  // 3xkk
    auto x = command & 0x0F00;
    auto kk = command & 0x00FF;
    if (this->registers[x] == kk) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if (command & 0xF000 == 0x4000) {  // 4xkk
    auto x = command & 0x0F00;
    auto kk = command & 0x00FF;
    if (this->registers[x] != kk) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x5000) {  // 5xy0
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    if (this->registers[x] == this->registers[y]) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if (command & 0xF000 == 0x6000) {  // 6xkk
    auto x = command & 0x0F00;
    auto kk = command & 0x00FF;
    this->registers[x] = kk;
    this->program_counter += 2;
  } else if (command & 0xF000 == 0x7000) {  // 7xkk
    auto x = command & 0x0F00;
    auto kk = command & 0x00FF;
    this->registers[x] += kk;
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8000) {  // 8xy0
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[x] = this->registers[y];
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8001) {  // 8xy1
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[x] |= this->registers[y];
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8002) {  // 8xy2
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[x] &= this->registers[y];
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8003) {  // 8xy3
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[x] ^= this->registers[y];
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8004) {  // 8xy4
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[Processor::REGISTER::VF] = 0;
    if (this->registers[x] + this->registers[y] < this->registers[x]) {
      this->registers[Processor::REGISTER::VF] = 1;
    }
    this->registers[x] += this->registers[y];
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8005) {  // 8xy5
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[Processor::VF] = 0;
    if (this->registers[x] - this->registers[y] > this->registers[x]) {
      this->registers[Processor::VF] = 1;
    }
    this->registers[x] -= this->registers[y];
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8006) {  // 8xy6
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[Processor::VF] = this->registers[x] & 0x1;
    this->registers[x] >>= 1;
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x8007) {  // 8xy7
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[Processor::REGISTER::VF] = 0;
    if (this->registers[y] - this->registers[x] > this->registers[y]) {
      this->registers[Processor::REGISTER::VF] = 1;
    }
    this->registers[x] = this->registers[y] - this->registers[x];
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x800E) {  // 8xyE
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    this->registers[Processor::REGISTER::VF] = this->registers[x] & 0x80;
    this->registers[x] <<= 1;
    this->program_counter += 2;
  } else if (command & 0xF00F == 0x9000) {  // 9xy0
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    if (this->registers[x] == this->registers[y]) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if (command & 0xF000 == 0xA000) {  // Annn
    auto nnn = command & 0x0FFF;
    this->index_register = nnn;
    this->program_counter += 2;
  } else if (command & 0xF000 == 0xB000) {  // Bnnn
    auto nnn = command & 0x0FFF;
    this->index_register =
        uint16_t(this->registers[Processor::REGISTER::V0]) + nnn;
    this->program_counter += 2;
  } else if (command & 0xF000 == 0xC000) {  // Cxkk
    auto x = command & 0x0F00;
    auto kk = command & 0x00FF;
    this->registers[x] = rand() & kk;
    this->program_counter += 2;
  } else if (command & 0xF000 == 0xD000) {  // Dxyn
    auto x = command & 0x0F00;
    auto y = command & 0x00F0;
    auto n = command & 0x000F;

    this->registers[Processor::REGISTER::VF] = 0;
    for (int i = 0; i < n; ++i) {
      auto sprite_byte = this->memory[this->index_register + i / 8];
      auto pixel = sprite_byte & (0x1 << (i % 8));
      auto xpos = (x + (i % 8)) % SCREEN_WIDTH;
      auto ypos = (y + (i / 8)) % SCREEN_HEIGHT;

      if (pixel && this->frame_buffer[ypos * SCREEN_WIDTH + xpos]) {
        this->registers[Processor::REGISTER::VF] = 1;
      }
      this->frame_buffer[ypos * SCREEN_WIDTH + xpos] ^= pixel;
    }
    this->program_counter += 2;
    return Code::DRW;
  } else if (command & 0xF09E == 0xE09E) {  // Ex9E
    auto x = command & 0x0F00;
    if (static_cast<uint16_t>(key) == uint16_t(this->registers[x])) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if (command & 0xF09E == 0xE0A1) {  // ExA1
    auto x = command & 0x0F00;
    if (static_cast<uint16_t>(key) != uint16_t(this->registers[x])) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF007) {  // Fx07
    auto x = command & 0x0F00;
    this->registers[x] = this->delay_timer;
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF00A) {  // Fx0A
    auto x = command & 0x0F00;
    if (key != Keyboard::NONE) {
      this->registers[x] = static_cast<uint8_t>(key);
      this->program_counter += 2;
    }
  } else if (command & 0xF0FF == 0xF015) {  // Fx15
    auto x = command & 0x0F00;
    this->delay_timer = this->registers[x];
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF018) {  // Fx18
    auto x = command & 0x0F00;
    this->sound_timer = this->registers[x];
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF01E) {  // Fx1E
    auto x = command & 0x0F00;
    this->index_register += this->registers[x];
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF029) {  // Fx29
    auto x = command & 0x0F00;
    this->index_register = this->registers[x] * 5;
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF033) {  // Fx33
    auto x = command & 0x0F00;
    // ???
    this->memory[this->index_register] = this->registers[x] / 100;
    this->memory[this->index_register + 1] = (this->registers[x] / 10) % 10;
    this->memory[this->index_register + 2] = this->registers[x] % 10;
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF055) {  // Fx55
    auto x = command & 0x0F00;
    for (int i = 0; i < x; ++i) {
      this->memory[this->index_register + i] = this->registers[i];
    }
    this->index_register += x + 1;
    this->program_counter += 2;
  } else if (command & 0xF0FF == 0xF065) {  // Fx65
    auto x = command & 0x0F00;
    for (int i = 0; i < x; ++i) {
      this->registers[i] = this->memory[this->index_register + i];
    }
    this->index_register += x + 1;
    this->program_counter += 2;
  }

  return Code::NONE;
}

void Processor::initialize_memory() {
  for (int i = 0; i < registers.size(); ++i) {
    registers[i] = 0;
  }
  for (int i = 0; i < stack.size(); ++i) {
    stack[i] = 0;
  }
  for (int i = 0; i < frame_buffer.size(); ++i) {
    frame_buffer[i] = 0;
  }
  for (int i = 0; i < memory.size(); ++i) {
    memory[i] = 0;
  }
}

void Processor::clean_display() {
  for (int i = 0; i < frame_buffer.size(); ++i) {
    frame_buffer[i] = 0;
  }
}

#include <iostream>
void Processor::initialize_font() {}

auto Processor::get_frame_buffer() -> frame_buff {
  this->clean_display();

  auto p1 = rand() % (SCREEN_HEIGHT * SCREEN_WIDTH);
  auto p2 = rand() % (SCREEN_HEIGHT * SCREEN_WIDTH);
  auto p3 = rand() % (SCREEN_HEIGHT * SCREEN_WIDTH);
  this->frame_buffer[p1] = 1;
  this->frame_buffer[p2] = 1;
  this->frame_buffer[p3] = 1;

  return this->frame_buffer;
}
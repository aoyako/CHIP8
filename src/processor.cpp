#include "processor.hpp"

#include <algorithm>
#include <iostream>

Processor::Processor() {
  this->initialize_memory();
  this->initialize_font();
}

void Processor::load_program(std::vector<uint8_t> program_data) {
  for (int i = 0; i < std::min(program_data.size(),
                               std::size_t(Processor::MEMORY_SIZE -
                                           Processor::PROGRAM_DATA_START));
       ++i) {
    this->memory[PROGRAM_DATA_START + i] = program_data[i];
  }
}

auto Processor::run(Keyboard key) -> Code {
  auto command = (uint16_t(this->memory[this->program_counter] << 8)) +
                 (uint16_t(this->memory[this->program_counter + 1]));
  std::cout << std::hex << std::showbase << int(command) << std::endl;

  if (command == 0x00E0) {  // 00E0
    this->clean_display();
    this->program_counter += 2;
    return Code::DRW;
  } else if ((command & 0xF000) == 0x7000) {  // 7xkk
    auto x = (command & 0x0F00) >> 8;
    auto kk = command & 0x00FF;
    this->registers[x] += kk;
    this->program_counter += 2;
  } else if (command == 0x00EE) {  // 00EE
    this->program_counter = this->stack[this->stack_pointer];
    this->stack_pointer--;
  } else if ((command & 0xF000) == 0x1000) {  // 1nnn
    this->program_counter = command & 0x0FFF;
  } else if ((command & 0xF000) == 0x2000) {  // 2nnn
    this->stack_pointer++;
    this->stack[this->stack_pointer] = this->program_counter;
    this->program_counter = command & 0x0FFF;
  } else if ((command & 0xF000) == 0x3000) {  // 3xkk
    auto x = (command & 0x0F00) >> 8;
    auto kk = command & 0x00FF;
    if (this->registers[x] == kk) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0x4000) {  // 4xkk
    auto x = (command & 0x0F00) >> 8;
    auto kk = command & 0x00FF;
    if (this->registers[x] != kk) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x5000) {  // 5xy0
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    if (this->registers[x] == this->registers[y]) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0x6000) {  // 6xkk
    auto x = (command & 0x0F00) >> 8;
    auto kk = command & 0x00FF;
    this->registers[x] = kk;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8000) {  // 8xy0
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[x] = this->registers[y];
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8001) {  // 8xy1
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[x] |= this->registers[y];
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8002) {  // 8xy2
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[x] &= this->registers[y];
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8003) {  // 8xy3
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[x] ^= this->registers[y];
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8004) {  // 8xy4
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[Processor::REGISTER::VF] = 0;
    if (this->registers[x] + this->registers[y] < this->registers[x]) {
      this->registers[Processor::REGISTER::VF] = 1;
    }
    this->registers[x] += this->registers[y];
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8005) {  // 8xy5
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[Processor::VF] = 0;
    if (this->registers[x] - this->registers[y] > this->registers[x]) {
      this->registers[Processor::VF] = 1;
    }
    this->registers[x] -= this->registers[y];
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8006) {  // 8xy6
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[Processor::VF] = this->registers[x] & 0x1;
    this->registers[x] >>= 1;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8007) {  // 8xy7
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[Processor::REGISTER::VF] = 0;
    if (this->registers[y] - this->registers[x] > this->registers[y]) {
      this->registers[Processor::REGISTER::VF] = 1;
    }
    this->registers[x] = this->registers[y] - this->registers[x];
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x800E) {  // 8xyE
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[Processor::REGISTER::VF] = this->registers[x] & 0x80;
    this->registers[x] <<= 1;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x9000) {  // 9xy0
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    if (this->registers[x] == this->registers[y]) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0xA000) {  // Annn
    auto nnn = command & 0x0FFF;
    this->index_register = nnn;
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0xB000) {  // Bnnn
    auto nnn = command & 0x0FFF;
    this->index_register =
        uint16_t(this->registers[Processor::REGISTER::V0]) + nnn;
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0xC000) {  // Cxkk
    auto x = (command & 0x0F00) >> 8;
    auto kk = command & 0x00FF;
    this->registers[x] = rand() & kk;
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0xD000) {  // Dxyn
    auto xx = (command & 0x0F00) >> 8;
    auto yy = (command & 0x00F0) >> 4;
    auto n = command & 0x000F;
    auto x = this->registers[xx];
    auto y = this->registers[yy];

    this->registers[Processor::REGISTER::VF] = 0;
    for (int i = 0; i < n; ++i) {
      auto sprite_byte = this->memory[this->index_register + i];
      std::cout << "putting sprite: " << int(sprite_byte) << std::endl;
      for (int b = 0; b < 8; ++b) {
        auto pixel = ((sprite_byte << b) & 0x80) >> 7;
        auto xpos = (x + b) % SCREEN_WIDTH;
        auto ypos = (y + i) % SCREEN_HEIGHT;

        if (pixel && this->frame_buffer[ypos * SCREEN_WIDTH + xpos]) {
          this->registers[Processor::REGISTER::VF] = 1;
        }
        this->frame_buffer[ypos * SCREEN_WIDTH + xpos] ^= pixel;
      }
    }
    // this->index_register += n;
    this->program_counter += 2;
    return Code::DRW;
  } else if ((command & 0xF09E) == 0xE09E) {  // Ex9E
    auto x = (command & 0x0F00) >> 8;
    if (static_cast<uint16_t>(key) == uint16_t(this->registers[x])) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if ((command & 0xF09E) == 0xE0A1) {  // ExA1
    auto x = (command & 0x0F00) >> 8;
    if (static_cast<uint16_t>(key) != uint16_t(this->registers[x])) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF007) {  // Fx07
    auto x = (command & 0x0F00) >> 8;
    this->registers[x] = this->delay_timer;
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF00A) {  // Fx0A
    auto x = (command & 0x0F00) >> 8;
    if (key != Keyboard::NONE) {
      this->registers[x] = static_cast<uint8_t>(key);
      this->program_counter += 2;
    }
  } else if ((command & 0xF0FF) == 0xF015) {  // Fx15
    auto x = (command & 0x0F00) >> 8;
    this->delay_timer = this->registers[x];
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF018) {  // Fx18
    auto x = (command & 0x0F00) >> 8;
    this->sound_timer = this->registers[x];
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF01E) {  // Fx1E
    auto x = (command & 0x0F00) >> 8;
    this->index_register += this->registers[x];
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF029) {  // Fx29
    auto x = (command & 0x0F00) >> 8;
    this->index_register = this->registers[x] * 5;
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF033) {  // Fx33
    auto x = (command & 0x0F00) >> 8;
    // ???
    this->memory[this->index_register] = this->registers[x] / 100;
    this->memory[this->index_register + 1] = (this->registers[x] / 10) % 10;
    this->memory[this->index_register + 2] = this->registers[x] % 10;
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF055) {  // Fx55
    auto x = (command & 0x0F00) >> 8;
    for (int i = 0; i < x; ++i) {
      this->memory[this->index_register + i] = this->registers[i];
    }
    this->index_register += x + 1;
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF065) {  // Fx65
    auto x = (command & 0x0F00) >> 8;
    for (int i = 0; i < x; ++i) {
      this->registers[i] = this->memory[this->index_register + i];
    }
    this->index_register += x + 1;
    this->program_counter += 2;
  } else {
    this->program_counter += 2;
    std::cerr << "unknow command" << std::endl;
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
  this->program_counter = PROGRAM_DATA_START;
}

void Processor::clean_display() {
  for (int i = 0; i < frame_buffer.size(); ++i) {
    frame_buffer[i] = 0;
  }
}

#include <iostream>
void Processor::initialize_font() {}

auto Processor::get_frame_buffer() -> frame_buff { return this->frame_buffer; }
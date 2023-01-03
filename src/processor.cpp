#include "processor.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

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

auto Processor::run(std::array<bool, 16> keys) -> Code {
  if (this->halted_key() != -1) {
    std::cout << "fucked" << std::endl;
    return Code::NONE;
  }

  auto command = (uint16_t(this->memory[this->program_counter] << 8)) +
                 (uint16_t(this->memory[this->program_counter + 1]));
  // std::cout << std::hex << std::showbase << int(command) << std::endl;

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
    this->program_counter = this->stack[this->stack_pointer] + 2;
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
    this->registers[Processor::REGISTER::VF] = 0;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8002) {  // 8xy2
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[x] &= this->registers[y];
    this->registers[Processor::REGISTER::VF] = 0;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8003) {  // 8xy3
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    this->registers[x] ^= this->registers[y];
    this->registers[Processor::REGISTER::VF] = 0;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8004) {  // 8xy4
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;

    auto last_vx_val = this->registers[x];
    this->registers[x] += this->registers[y];
    if (this->registers[x] < last_vx_val) {
      this->registers[Processor::REGISTER::VF] = 1;
    } else {
      this->registers[Processor::REGISTER::VF] = 0;
    }
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8005) {  // 8xy5
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;

    auto last_vx_val = this->registers[x];
    this->registers[x] -= this->registers[y];
    if (last_vx_val < this->registers[y]) {
      this->registers[Processor::REGISTER::VF] = 0;
    } else {
      this->registers[Processor::REGISTER::VF] = 1;
    }
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8006) {  // 8xy6
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;

    this->registers[x] = this->registers[y];
    auto last_vx_val = this->registers[x];
    this->registers[x] >>= 1;
    this->registers[Processor::VF] = last_vx_val & 0x1;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x8007) {  // 8xy7
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;

    auto last_vx_val = this->registers[x];
    this->registers[x] = this->registers[y] - this->registers[x];
    if (this->registers[y] > last_vx_val) {
      this->registers[Processor::REGISTER::VF] = 1;
    } else {
      this->registers[Processor::REGISTER::VF] = 0;
    }
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x800E) {  // 8xyE
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;

    this->registers[x] = this->registers[y];
    auto last_vx_val = this->registers[x];
    this->registers[x] <<= 1;
    this->registers[Processor::REGISTER::VF] = (last_vx_val & 0x80) >> 7;
    this->program_counter += 2;
  } else if ((command & 0xF00F) == 0x9000) {  // 9xy0
    auto x = (command & 0x0F00) >> 8;
    auto y = (command & 0x00F0) >> 4;
    if (this->registers[x] != this->registers[y]) {
      this->program_counter += 2;
    }
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0xA000) {  // Annn
    auto nnn = command & 0x0FFF;
    this->index_register = nnn;
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0xB000) {  // Bnnn
    auto x = (command & 0x0F00) >> 8;
    auto nnn = command & 0x0FFF;
    this->program_counter =
        uint16_t(this->registers[Processor::REGISTER::V0]) + nnn;
  } else if ((command & 0xF000) == 0xC000) {  // Cxkk
    auto x = (command & 0x0F00) >> 8;
    auto kk = command & 0x00FF;
    this->registers[x] = rand() & kk;
    this->program_counter += 2;
  } else if ((command & 0xF000) == 0xD000) {  // Dxyn
    this->frames_updated = true;
    auto xx = (command & 0x0F00) >> 8;
    auto yy = (command & 0x00F0) >> 4;
    auto n = command & 0x000F;
    auto x = this->registers[xx] & 0x3F;
    auto y = this->registers[yy] & 0x1F;

    this->registers[Processor::REGISTER::VF] = 0;
    for (int i = 0; i < n; ++i) {
      auto sprite_byte = this->memory[this->index_register + i];
      for (int b = 0; b < 8; ++b) {
        auto pixel = ((sprite_byte << b) & 0x80) >> 7;
        auto xpos = (x + b);
        auto ypos = (y + i);
        if ((xpos >= SCREEN_WIDTH) || (ypos >= SCREEN_HEIGHT)) {
          continue;
        }

        if (pixel && this->frame_buffer[ypos * SCREEN_WIDTH + xpos]) {
          this->registers[Processor::REGISTER::VF] = 1;
        }
        this->frame_buffer[ypos * SCREEN_WIDTH + xpos] ^= pixel;
      }
    }
    // this->index_register += n;
    this->program_counter += 2;
    // return Code::DRW;
  } else if ((command & 0xF0FF) == 0xE09E) {  // Ex9E
    auto x = (command & 0x0F00) >> 8;
    if (!(this->registers[x] >= keys.size())) {
      if (keys[this->registers[x]]) {
        this->program_counter += 2;
      }
    }
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xE0A1) {  // ExA1
    auto x = (command & 0x0F00) >> 8;
    if (!(this->registers[x] >= keys.size())) {
      if (!keys[this->registers[x]]) {
        this->program_counter += 2;
      }
    }
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF007) {  // Fx07
    auto x = (command & 0x0F00) >> 8;
    this->registers[x] = this->delay_timer;
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF00A) {  // Fx0A
    auto x = (command & 0x0F00) >> 8;
    if (std::reduce(keys.begin(), keys.end(), false,
                    [](const auto &x, const auto &y) { return x || y; })) {
      // this->blocked_key = keys[0];
      this->registers[x] = static_cast<uint8_t>(this->blocked_key);
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
    for (int i = 0; i <= x; ++i) {
      this->memory[this->index_register + i] = this->registers[i];
    }
    this->index_register += x + 1;
    this->program_counter += 2;
  } else if ((command & 0xF0FF) == 0xF065) {  // Fx65
    auto x = (command & 0x0F00) >> 8;
    for (int i = 0; i <= x; ++i) {
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

void Processor::update_delay_timer() {
  if (this->delay_timer != 0) {
    this->delay_timer--;
  }
}

void Processor::update_sound_timer() {
  if (this->sound_timer != 0) {
    this->sound_timer--;
  }
}

auto Processor::should_draw() -> bool { return this->frames_updated; }

void Processor::clean_frames() { this->frames_updated = false; }

auto Processor::halted_key() -> int { return this->blocked_key; }

void Processor::release_key() { this->blocked_key = -1; }

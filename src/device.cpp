#include "device.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

namespace Device {

constexpr static int TICKS_PER_ITER = 10;

Device::Device(const std::string &rom_path, const std::string &audio_path)
    : display(Processor::SCREEN_WIDTH, Processor::SCREEN_HEIGHT, "chip8"),
      audio(audio_path) {
  std::ifstream input(rom_path, std::ios::binary);
  if (input.is_open()) {
    input.seekg(0, std::ios::end);
    std::streampos size = input.tellg();
    input.seekg(0, std::ios::beg);

    std::vector<char> program_raw(size);
    input.read(program_raw.data(), size);

    std::vector<uint8_t> program;
    std::copy(program_raw.begin(), program_raw.end(),
              std::back_inserter(program));
    this->processor.load_program(program);

    input.close();
  } else {
    throw std::runtime_error("Could not open file: " + rom_path);
  }
}

void Device::run() {
  this->display.draw(this->processor.get_frame_buffer());
  const auto key_labels = std::array<SDL_Scancode, 16>{
      SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
      SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
      SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
      SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V,
  };

  int tick = 0;
  int close = 0;
  while (!close) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          close = 1;
          break;
      }
    }

    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    auto keys = std::array<bool, 16>{};

    for (int i = 0; i < key_labels.size(); ++i) {
      keys[i] = false;
      if (state[key_labels[i]]) {
        keys[i] = true;
      }
    }

    if (this->processor.halted_key() != -1) {
      if (!keys[this->processor.halted_key()]) {
        this->processor.release_key();
      } else {
        continue;
      }
    }
    auto code = this->processor.run(keys);

    if (code == Processor::Code::DRW) {
      this->display.draw(this->processor.get_frame_buffer());
    }

    if (tick == TICKS_PER_ITER) {
      tick = 0;

      if (this->processor.should_draw()) {
        this->display.draw(this->processor.get_frame_buffer());
      }

      if (this->processor.get_sound_timer()) {
        this->audio.play();
      }

      this->processor.update_delay_timer();
      this->processor.update_sound_timer();
    }
    tick++;
    SDL_Delay(1);
  }
}
}  // namespace Device
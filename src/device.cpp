#include "device.hpp"

#include <fstream>
#include <iostream>
#include <iterator>

Device::Device(const std::string& filename)
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, "chip8") {
  std::ifstream input(filename, std::ios::binary);
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
    throw std::runtime_error("Could not open file: " + filename);
  }
}

void Device::run() {
  this->display.draw(this->processor.get_frame_buffer());
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

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    auto key = Keyboard::NONE;

    if (state[SDL_SCANCODE_1]) {
      key = Keyboard::K0;
    } else if (state[SDL_SCANCODE_2]) {
      key = Keyboard::K1;
    } else if (state[SDL_SCANCODE_3]) {
      key = Keyboard::K2;
    } else if (state[SDL_SCANCODE_4]) {
      key = Keyboard::K3;
    } else if (state[SDL_SCANCODE_Q]) {
      key = Keyboard::K4;
    } else if (state[SDL_SCANCODE_W]) {
      key = Keyboard::K5;
    } else if (state[SDL_SCANCODE_E]) {
      key = Keyboard::K6;
    } else if (state[SDL_SCANCODE_R]) {
      key = Keyboard::K7;
    } else if (state[SDL_SCANCODE_A]) {
      key = Keyboard::K8;
    } else if (state[SDL_SCANCODE_S]) {
      key = Keyboard::K9;
    } else if (state[SDL_SCANCODE_D]) {
      key = Keyboard::KA;
    } else if (state[SDL_SCANCODE_F]) {
      key = Keyboard::KB;
    } else if (state[SDL_SCANCODE_Z]) {
      key = Keyboard::KC;
    } else if (state[SDL_SCANCODE_X]) {
      key = Keyboard::KD;
    } else if (state[SDL_SCANCODE_C]) {
      key = Keyboard::KE;
    } else if (state[SDL_SCANCODE_V]) {
      key = Keyboard::KF;
    }

    // std::cout << static_cast<int>(key) << std::endl;

    auto code = this->processor.run(key);

    if (code == Code::DRW) {
      this->display.draw(this->processor.get_frame_buffer());
    }

    if (tick == 60) {
      tick = 0;
      this->display.draw(this->processor.get_frame_buffer());
      this->processor.update_delay_timer();
      this->processor.update_sound_timer();
    }
    tick++;
    SDL_Delay(1);
  }
}
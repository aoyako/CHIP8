#include "device.hpp"

#include <iostream>

Device::Device(const std::string& filename)
    : display(Processor::SCREEN_WIDTH, Processor::SCREEN_HEIGHT, "chip8") {}

void Device::run() {
  int close = 0;
  while (!close) {
    this->display.draw();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          close = 1;
          break;
      }

      const Uint8* state = SDL_GetKeyboardState(nullptr);
      auto key = Keyboard::NONE;

      if (state[SDL_SCANCODE_7]) {
        key = Keyboard::K0;
      } else if (state[SDL_SCANCODE_8]) {
        key = Keyboard::K1;
      } else if (state[SDL_SCANCODE_9]) {
        key = Keyboard::K2;
      } else if (state[SDL_SCANCODE_C]) {
        key = Keyboard::K3;
      } else if (state[SDL_SCANCODE_4]) {
        key = Keyboard::K4;
      } else if (state[SDL_SCANCODE_5]) {
        key = Keyboard::K5;
      } else if (state[SDL_SCANCODE_6]) {
        key = Keyboard::K6;
      } else if (state[SDL_SCANCODE_D]) {
        key = Keyboard::K7;
      } else if (state[SDL_SCANCODE_1]) {
        key = Keyboard::K8;
      } else if (state[SDL_SCANCODE_2]) {
        key = Keyboard::K9;
      } else if (state[SDL_SCANCODE_3]) {
        key = Keyboard::KA;
      } else if (state[SDL_SCANCODE_E]) {
        key = Keyboard::KB;
      } else if (state[SDL_SCANCODE_A]) {
        key = Keyboard::KC;
      } else if (state[SDL_SCANCODE_0]) {
        key = Keyboard::KD;
      } else if (state[SDL_SCANCODE_B]) {
        key = Keyboard::KE;
      } else if (state[SDL_SCANCODE_F]) {
        key = Keyboard::KF;
      }

      std::cout << static_cast<int>(key) << std::endl;

      auto code = this->processor.run(key);
    }

    SDL_Delay(1000 / 60);
  }
}
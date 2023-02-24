#include "display.hpp"

#include <stdexcept>

namespace Display {

constexpr static int DEFAULT_AUDIO_RATE = 44100;
constexpr static int DEFAULT_AUDIO_BUFF_SIZE = 2048;

Display::Display(int width, int height, const std::string& title) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    throw std::runtime_error("SDL init error: " + std::string(SDL_GetError()));
  }

  this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, width * SCALE,
                                  height * SCALE, 0);

  this->renderer = SDL_CreateRenderer(window, -1, 0);
}

void Display::draw(Processor::frame_buff buffer) const {
  SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
  SDL_RenderClear(this->renderer);

  for (int y = 0; y < Processor::SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < Processor::SCREEN_WIDTH; ++x) {
      if (buffer[y * Processor::SCREEN_WIDTH + x]) {
        SDL_Rect r;
        r.x = x * SCALE;
        r.y = y * SCALE;
        r.w = SCALE;
        r.h = SCALE;

        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(this->renderer, &r);
      }
    }
  }

  SDL_RenderPresent(this->renderer);
}

Display::~Display() {
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}
}  // namespace Display

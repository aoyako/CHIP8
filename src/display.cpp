#include "display.hpp"

#include <stdexcept>

Display::Display(int width, int height, const std::string &title) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    throw std::runtime_error("SDL init error: " + std::string(SDL_GetError()));
  }

  this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 512, 512, 0);

  this->renderer = SDL_CreateRenderer(window, -1, 0);
}

void Display::draw() const {
  SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
  SDL_RenderClear(this->renderer);

  SDL_RenderPresent(this->renderer);
}
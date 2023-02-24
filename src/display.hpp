#pragma once

#include <SDL2/SDL.h>

#include <array>
#include <string>

#include "processor.hpp"

namespace Display {

constexpr static int SCALE = 10;

class Display {
 public:
  Display(int width, int height, const std::string& title);
  ~Display();
  void draw(Processor::frame_buff buffer) const;

 private:
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
};
}  // namespace Display
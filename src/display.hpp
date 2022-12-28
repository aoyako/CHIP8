#pragma once

#include <SDL2/SDL.h>

#include <string>

class Display {
 public:
  Display(int width, int height, const std::string& title);
  // void draw() const;

 private:
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
};
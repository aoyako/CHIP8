#pragma once

#include <SDL2/SDL.h>

#include <array>
#include <string>

#include "processor.hpp"

constexpr static int SCALE = 10;

class Display {
 public:
  Display(int width, int height, const std::string& title);
  ~Display();
  void draw(frame_buff buffer) const;

 private:
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
};

class Audio {
 public:
  Audio(const std::string& filename);
  ~Audio();
  void play() const;

 private:
  SDL_AudioSpec spec;
  uint32_t length;
  uint8_t* buffer = nullptr;
};
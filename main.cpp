#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#include <iostream>
#include <stdexcept>

#include "src/device.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "provide a chip8 ROM" << std::endl;
    return EXIT_FAILURE;
  }
  Device device(argv[1]);
  device.run();

  return EXIT_SUCCESS;
}

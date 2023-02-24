#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#include <stdexcept>

#include "src/device.hpp"

int main(int argc, char* argv[]) {
  Device device("roms/invaders.ch8");
  device.run();

  return 0;
}

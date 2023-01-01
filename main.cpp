#include <SDL2/SDL.h>
#include <stdio.h>

#include "src/device.hpp"

int main(int argc, char* argv[]) {
  Device device("roms/test.ch8");
  device.run();
  return 0;
}

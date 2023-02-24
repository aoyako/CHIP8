#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>

#include "src/device.hpp"

const std::string DEFAULT_AUDIO_LOCATION = "res/beep.wav";

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "provide a chip8 ROM" << std::endl;
    return EXIT_FAILURE;
  }

  auto audio_path = DEFAULT_AUDIO_LOCATION;
  if (argc >= 3) {
    audio_path = std::string(argv[2]);
  }

  Device device(argv[1], audio_path);
  device.run();

  return EXIT_SUCCESS;
}

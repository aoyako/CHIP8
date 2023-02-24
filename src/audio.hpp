#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string>

namespace Audio {

class Audio {
 public:
  Audio(const std::string& filename);
  ~Audio();
  void play() const;

 private:
  Mix_Chunk* sound = nullptr;
};

}  // namespace Audio
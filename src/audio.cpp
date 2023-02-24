#include "audio.hpp"

#include <stdexcept>

namespace Audio {

constexpr static int DEFAULT_AUDIO_RATE = 44100;
constexpr static int DEFAULT_AUDIO_BUFF_SIZE = 2048;

Audio::Audio(const std::string& filename) {
  Mix_OpenAudio(DEFAULT_AUDIO_RATE, MIX_DEFAULT_FORMAT, 2,
                DEFAULT_AUDIO_BUFF_SIZE);
  this->sound = Mix_LoadWAV(filename.c_str());
  if (this->sound == NULL) {
    throw std::runtime_error("Mixer audio load error: " +
                             std::string(Mix_GetError()));
  }
}

Audio::~Audio() {
  Mix_FreeChunk(this->sound);
  Mix_CloseAudio();
}

void Audio::play() const {
  if (!Mix_Playing(0)) {
    int channel = Mix_PlayChannel(-1, this->sound, 0);
    if (channel == -1) {
      throw std::runtime_error("Mixer play error: " +
                               std::string(Mix_GetError()));
    }
  }
}
}  // namespace Audio
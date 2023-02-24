#include "display.hpp"

#include <stdexcept>

constexpr static int DEFAULT_AUDIO_RATE = 44100;
constexpr static int DEFAULT_AUDIO_BUFF_SIZE = 2048;

Display::Display(int width, int height, const std::string& title) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    throw std::runtime_error("SDL init error: " + std::string(SDL_GetError()));
  }

  this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, width * SCALE,
                                  height * SCALE, 0);

  this->renderer = SDL_CreateRenderer(window, -1, 0);
}

void Display::draw(frame_buff buffer) const {
  SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
  SDL_RenderClear(this->renderer);

  for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
      if (buffer[y * SCREEN_WIDTH + x]) {
        SDL_Rect r;
        r.x = x * SCALE;
        r.y = y * SCALE;
        r.w = SCALE;
        r.h = SCALE;

        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(this->renderer, &r);
      }
    }
  }

  SDL_RenderPresent(this->renderer);
}

Display::~Display() {
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}

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
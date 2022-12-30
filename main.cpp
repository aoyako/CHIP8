#include <SDL2/SDL.h>
#include <stdio.h>

#include "src/device.hpp"

int main(int argc, char* argv[]) {
  Device device("roms/invaders.ch8");
  device.run();
  return 0;

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    return 1;
  }

  window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 512, 512, 0);

  renderer = SDL_CreateRenderer(window, -1, 0);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_Rect r;
  r.x = 50;
  r.y = 50;
  r.w = 50;
  r.h = 50;

  // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  // SDL_RenderFillRect(renderer, &r);

  // SDL_RenderPresent(renderer);

  int close = 0;
  while (!close) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255 * rand(), 255 * rand());
    SDL_RenderFillRect(renderer, &r);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          close = 1;
          break;
      }
    }

    // calculates to 60 fps
    SDL_Delay(1000 / 60);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

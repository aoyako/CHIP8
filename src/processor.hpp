#pragma once

#include <array>

class Processor {
 public:
  Processor();
  void run();

 private:
  const static int16_t SCREEN_WIDTH = 32;
  const static int16_t SCREEN_HEIGHT = 64;
  const static int16_t MEMORY_SIZE = 4096;

  uint16_t index_register;
  uint16_t program_counter;
  uint8_t delay_timer;
  uint8_t sound_timer;

  std::array<uint8_t, 16> registers;
  std::array<uint8_t, 8> stack;
  std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> frame_buffer;
  std::array<uint8_t, MEMORY_SIZE> memory;

  void initialize_memory();
  void initialize_font();
};
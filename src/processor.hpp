#pragma once

#include <array>
#include <cstdint>
#include <vector>

constexpr static int16_t SCREEN_WIDTH = 64;
constexpr static int16_t SCREEN_HEIGHT = 32;

using frame_buff = std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT>;

enum class Code {
  NONE,
  DRW,
};

enum class Keyboard {
  K0,
  K1,
  K2,
  K3,
  K4,
  K5,
  K6,
  K7,
  K8,
  K9,
  KA,
  KB,
  KC,
  KD,
  KE,
  KF,
  NONE,
};

class Processor {
 public:
  Processor();
  auto run(Keyboard key) -> Code;
  void load_program(std::vector<uint8_t> program_data);

  auto get_frame_buffer() -> frame_buff;

  void update_delay_timer();
  void update_sound_timer();

  auto should_draw() -> bool;
  void clean_frames();

 private:
  constexpr static int16_t MEMORY_SIZE = 4096;
  constexpr static int16_t PROGRAM_DATA_START = 0x200;

  enum REGISTER {
    V0 = 0x0,
    VF = 0xF,
  };

  uint16_t index_register = 0;
  uint8_t stack_pointer = 0;
  uint16_t program_counter = 0;
  uint8_t delay_timer = 0;
  uint8_t sound_timer = 0;

  bool frames_updated = false;

  std::array<uint8_t, 16> registers;
  std::array<uint16_t, 8> stack;
  std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> frame_buffer;
  std::array<uint8_t, MEMORY_SIZE> memory;

  void initialize_memory();
  void initialize_font();
  void clean_display();
};

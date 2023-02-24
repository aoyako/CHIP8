#pragma once

#include "display.hpp"
#include "processor.hpp"

class Device {
 public:
  Device(const std::string &rom_path, const std::string &audio_path);
  void run();

 private:
  Display display;
  Processor processor;
  Audio audio;
};
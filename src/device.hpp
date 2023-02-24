#pragma once

#include "audio.hpp"
#include "display.hpp"
#include "processor.hpp"

namespace Device {

class Device {
 public:
  Device(const std::string &rom_path, const std::string &audio_path);
  void run();

 private:
  Display::Display display;
  Processor::Processor processor;
  Audio::Audio audio;
};
}  // namespace Device
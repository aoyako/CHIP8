#pragma once

#include "display.hpp"
#include "processor.hpp"

class Device {
 public:
  Device(const std::string &filename);
  void run();

 private:
  Display display;
  Processor processor;
  Audio audio;
};
#pragma once
#include "esphome.h"
#include <vector>

class LD2410Tracker : public esphome::Component {
 public:
  LD2410Tracker() : uart_(nullptr) {}

  void set_uart(esphome::uart::UARTComponent *uart) { uart_ = uart; }

  void setup() override {}
  void loop() override;

 protected:
  esphome::uart::UARTComponent *uart_;

  bool parse_frame(std::vector<uint8_t> &stream,
                   bool &detected,
                   float &angle_deg,
                   float &dist_cm);
};

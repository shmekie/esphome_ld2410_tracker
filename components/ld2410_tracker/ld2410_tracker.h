#pragma once
#include "esphome.h"
#include <vector>
#include <functional>

class LD2410Tracker : public esphome::Component {
 public:
  void set_uart(esphome::uart::UARTComponent *uart) { uart_ = uart; }

  void set_update_object_present(std::function<void(bool)> f) { update_object_present_ = f; }
  void set_update_angle(std::function<void(float)> f) { update_angle_ = f; }
  void set_update_distance(std::function<void(float)> f) { update_distance_ = f; }
  void set_move_stepper(std::function<void(float)> f) { move_stepper_ = f; }

  void loop() override;

 protected:
  esphome::uart::UARTComponent *uart_{nullptr};

  std::function<void(bool)> update_object_present_;
  std::function<void(float)> update_angle_;
  std::function<void(float)> update_distance_;
  std::function<void(float)> move_stepper_;

  bool parse_frame(std::vector<uint8_t> &stream,
                   bool &detected,
                   float &angle_deg,
                   float &dist_cm);
};

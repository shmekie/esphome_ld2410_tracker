#include "ld2410_tracker.h"

using namespace esphome;

bool LD2410Tracker::parse_frame(std::vector<uint8_t> &stream,
                                bool &detected,
                                float &angle_deg,
                                float &dist_cm) {
  const uint8_t HEADER[4] = {0xF4, 0xF3, 0xF2, 0xF1};
  const uint8_t TAIL[4]   = {0xF8, 0xF7, 0xF6, 0xF5};

  if (stream.size() < 12)
    return false;

  // Ensure header alignment
  if (!(stream[0] == HEADER[0] &&
        stream[1] == HEADER[1] &&
        stream[2] == HEADER[2] &&
        stream[3] == HEADER[3])) {

    size_t pos = 0;
    for (size_t i = 1; i + 3 < stream.size(); i++) {
      if (stream[i] == HEADER[0] &&
          stream[i+1] == HEADER[1] &&
          stream[i+2] == HEADER[2] &&
          stream[i+3] == HEADER[3]) {
        pos = i;
        break;
      }
    }

    if (pos == 0) {
      if (stream.size() > 3)
        stream.erase(stream.begin(), stream.end() - 3);
      return false;
    } else {
      stream.erase(stream.begin(), stream.begin() + pos);
    }

    if (stream.size() < 12)
      return false;
  }

  // Length
  uint16_t len = (uint16_t)stream[4] | ((uint16_t)stream[5] << 8);
  size_t full_len = 6 + len + 4;

  if (stream.size() < full_len)
    return false;

  // Tail check
  if (!(stream[full_len-4] == TAIL[0] &&
        stream[full_len-3] == TAIL[1] &&
        stream[full_len-2] == TAIL[2] &&
        stream[full_len-1] == TAIL[3])) {

    stream.erase(stream.begin());
    return false;
  }

  size_t p = 6;

  // Normal data frame
  uint8_t type = stream[p++];
  if (type != 0x02) {
    stream.erase(stream.begin(), stream.begin() + full_len);
    return false;
  }

  uint8_t aa = stream[p++];
  if (aa != 0xAA) {
    stream.erase(stream.begin(), stream.begin() + full_len);
    return false;
  }

  if (p + 10 > full_len) {
    stream.erase(stream.begin(), stream.begin() + full_len);
    return false;
  }

  uint8_t target_state = stream[p++];  // 0=none, 1=motion, 2=static, 3=both

  uint16_t motion_dist = (uint16_t)stream[p] | ((uint16_t)stream[p+1] << 8);
  p += 2;
  uint8_t motion_energy = stream[p++];

  uint16_t static_dist = (uint16_t)stream[p] | ((uint16_t)stream[p+1] << 8);
  p += 2;
  uint8_t static_energy = stream[p++];

  uint16_t detect_dist = (uint16_t)stream[p] | ((uint16_t)stream[p+1] << 8);
  p += 2;

  // End bytes (0x55, 0x00)
  p += 2;

  stream.erase(stream.begin(), stream.begin() + full_len);

  // Fill data
  detected = (target_state != 0x00);
  dist_cm = (float) detect_dist;

  // Heuristic angle mapping
  float energy_diff = (float)motion_energy - (float)static_energy;
  float angle = 45.0f + (energy_diff * 0.6f);

  if (angle < 0.0f) angle = 0.0f;
  if (angle > 90.0f) angle = 90.0f;
  angle_deg = angle;

  return true;
}

void LD2410Tracker::loop() {
  if (uart_ == nullptr)
    return;

  size_t avail = uart_->available();
  if (avail == 0)
    return;

  uint8_t buf[512];
  if (avail > sizeof(buf)) avail = sizeof(buf);
  int read = uart_->read_array(buf, avail);
  if (read <= 0)
    return;

  static std::vector<uint8_t> stream;
  stream.insert(stream.end(), buf, buf + read);

  while (true) {
    bool detected = false;
    float angle = 0.0f;
    float dist = 0.0f;

    if (!parse_frame(stream, detected, angle, dist))
      break;

    // Update ESPHome global variables
    id(object_present) = detected ? 1 : 0;
    id(object_angle_deg) = angle;
    id(object_distance_cm) = dist;

    // Move stepper if tracking is enabled
    if (detected && id(tracking_enabled)) {
      float target_angle = angle;

      if (target_angle < id(field_start_deg))
        target_angle = id(field_start_deg);
      if (target_angle > id(field_end_deg))
        target_angle = id(field_end_deg);

      int steps = (int) roundf(target_angle * id(steps_per_degree));

      id(tracker_stepper).set_target(steps);
      id(current_angle_deg) = target_angle;
    }
  }
}

# ESPHome LD2410 Tracker (esphome_ld2410_tracker)

Custom ESPHome external component for the HLK-LD2410 mmWave radar that:
- Reads the LD2410 binary UART protocol
- Exposes presence, distance, and a heuristic "angle"
- Optionally drives a stepper (e.g. ULN2003 + 28BYJ-48) to track a target

Designed for use from ESPHome **external_components** via:

```yaml
external_components:
  - source: github://shmekie/esphome_ld2410_tracker

ld2410_tracker:
  uart_id: uart_bus

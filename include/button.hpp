#pragma once

#include <Arduino.h>
#include <stdint.h>

class Button {
public:
  enum State { Down = 0, Up = 1 };

  Button(uint8_t pin, uint8_t debounce_ms = 5);

  void init();

  bool pressed();

private:
  inline State pin_state() const {
    return static_cast<Button::State>(digitalRead(m_pin));
  }

  uint8_t m_pin;
  uint8_t m_debounce_ms;

  State m_state;

  State m_last_read;
  uint32_t m_last_read_ms;
};

class Keypad {

public:
  Keypad(uint8_t mode_pin, uint8_t set_pin, uint8_t down_pin, uint8_t up_pin);

  void init();

private:
  // Switch mode/trigger backlight
  Button mode_button;
  // Rotate through set modes
  Button set_button;
  // Move down
  Button down_button;
  // Move up
  Button up_button;
};
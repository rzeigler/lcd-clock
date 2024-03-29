#include <Arduino.h>
#include <button.hpp>
#include <util.hpp>

Button::Button(uint8_t pin, uint8_t debounce_ms)
    // Assume the buttons all start unpressed
    : m_pin(pin), m_debounce_ms(debounce_ms), m_state(State::Up),
      m_last_read(State::Up) {}

void Button::init() {
  pinMode(m_pin, INPUT_PULLUP);
  m_last_read_ms = millis();
}

bool Button::pressed() {
  /* Immediate mode pressed with debounce logic
   * Assumes this will be called as at a high rate of speed.
   * Handles debounce internally
   */
  uint32_t current_ms = millis();
  State current_read = pin_state();

  // State change, reset the debounce timer
  if (current_read != m_last_read) {
    m_last_read = current_read;
    m_last_read_ms = current_ms;
    // No button press+release, still debouncing
    return false;
  }

  if (diff_millis(m_last_read_ms, current_ms) < m_debounce_ms) {
    // Not enough time has elapsed
    return false;
  }

  // The current state of the pin matches our current state tracking, nothing
  // has changed
  if (m_last_read == m_state) {
    return false;
  }
  // At this point, the button state doesn't match the state machine
  // Internal state is down, but we are reading up, switch internal state
  // and return true because full cycle has gone through
  if (m_last_read == State::Up) {
    m_state = State::Up;
    return true;
  } else {
    // Otherwise, mark as down and we will cycle back through until the button
    // is lifted
    m_state = State::Down;
    return false;
  }
}

const int MODE_BUTTON = 0;
const int SET_BUTTON = 1;
const int DOWN_BUTTON = 2;
const int UP_BUTTON = 3;

Keypad::Keypad(uint8_t mode_pin, uint8_t set_pin, uint8_t down_pin,
               uint8_t up_pin)
    : m_buttons({Button(mode_pin), Button(set_pin), Button(down_pin),
                 Button(up_pin)}),
      m_last_poll{false, false, false, false} {}

void Keypad::Keypad::init() {
  for (int i = 0; i < 4; ++i) {
    m_buttons[i].init();
  }
}

void Keypad::Keypad::poll() {
  for (int i = 0; i < 4; ++i) {
    m_last_poll[i] = m_buttons[i].pressed();
  }
}

bool Keypad::Keypad::button_pressed() const {
  for (int i = 0; i < 4; ++i) {
    if (m_last_poll[i]) {
      return true;
    }
  }
  return false;
}

bool Keypad::Keypad::mode_pressed() const { return m_last_poll[MODE_BUTTON]; }

bool Keypad::Keypad::set_pressed() const { return m_last_poll[SET_BUTTON]; }

bool Keypad::Keypad::down_pressed() const { return m_last_poll[DOWN_BUTTON]; }

bool Keypad::Keypad::up_pressed() const { return m_last_poll[UP_BUTTON]; }
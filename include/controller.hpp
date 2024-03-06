#pragma once

#include <button.hpp>
#include <clock.hpp>
#include <display.hpp>
#include <ui.hpp>

constexpr unsigned int fps_to_interval(unsigned int fps) { return 1000 / fps; }

class Controller {
public:
  Controller(ClockModel &clock, Display &display, Keypad &keypad,
             unsigned int redraw_interval, unsigned int clock_refresh_interval,
             unsigned int backlight_duration_ms);

  void init(unsigned long now_ms);

  void tick(unsigned long now_ms);

private:
  void process_inputs(unsigned long now_ms);
  void refresh_clock(unsigned long now_ms);
  void draw(unsigned long now_ms);

  const unsigned int m_clock_refresh_interval_ms;
  const unsigned int m_redraw_interval_ms;
  const unsigned int m_backlight_duration_ms;

  // Split these out for testing
  ClockModel &m_clock;
  Display &m_display;
  Keypad &m_keypad;

  unsigned long m_last_redraw_ms;
  unsigned long m_last_clock_refresh_ms;

  DecayingFlag m_backlight_flag;

  Grid m_ui_grid;
};
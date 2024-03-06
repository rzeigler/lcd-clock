#pragma once

#include <button.hpp>
#include <clock.hpp>
#include <display.hpp>
#include <ui.hpp>
#include <util.hpp>

constexpr unsigned int fps_to_interval(unsigned int fps) { return 1000 / fps; }

template <typename T> class WrappingInt {
public:
  WrappingInt(T init, T low_inc, T high_inc)
      : m_low_inc(low_inc), m_high_inc(high_inc) {
    if (low_inc > high_inc) {
      panic("invalid wrapping int");
    }
    set(init);
  }

  void set(T value) {
    if (value < m_low_inc) {
      m_value = m_low_inc;
    } else if (value > m_high_inc) {
      m_value = m_high_inc;
    } else {
      m_value = value;
    }
  }

  void incr() {
    m_value++;
    if (m_value > m_high_inc) {
      m_value = m_low_inc;
    }
  }

  void decr() {
    m_value--;
    if (m_value < m_low_inc) {
      m_value = m_high_inc;
    }
  }

  inline T get() const { return m_value; }

private:
  T m_value;
  T m_low_inc;
  T m_high_inc;
};

class SetTimePage : public Animating {
public:
  explicit SetTimePage(unsigned int blink_interval_ms);

  void reset(const Time &init_time);

  virtual void start(unsigned long now_ms) override;
  virtual void age(unsigned long now_ms) override;

  void on_mode();
  void on_down();
  void on_up();

  void draw(Grid &ui_grid) const;

private:
  enum class EditFocus { hour, minute, second, ampm };

  RepeatingStepAnimation m_blink;
  EditFocus m_focus;

  WrappingInt<uint8_t> m_hour;
  WrappingInt<uint8_t> m_minute;
  WrappingInt<uint8_t> m_second;
  bool m_is_pm;
};

class Controller {
public:
  Controller(ClockModel &clock, Display &display, Keypad &keypad,
             unsigned int redraw_interval, unsigned int clock_refresh_interval);

  void init(unsigned long now_ms);

  void tick(unsigned long now_ms);

private:
  enum class Page { display, set_clock, set_alarm, set_warmup };

  void process_inputs(unsigned long now_ms);
  void on_set();

  void process_animations(unsigned long now_ms);
  void refresh_clock(unsigned long now_ms);
  void draw(unsigned long now_ms);

  // Split these out for testing
  ClockModel &m_clock;
  Display &m_display;
  Keypad &m_keypad;

  RefreshInterval m_redraw_interval;
  RefreshInterval m_clock_interval;

  OneShotStepAnimation m_backlight_flag;

  // TODO: Construct a virtual page controller with enter/exit/handle
  // mode/up/down
  Page m_display_state;

  SetTimePage m_set_clock;
  SetTimePage m_set_alarm;

  Animating *m_animates[3];
  // Scratch for the display so we aren't constantly recreating
  Grid m_ui_grid;
};

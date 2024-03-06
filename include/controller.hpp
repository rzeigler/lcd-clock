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

class Screen {
public:
  virtual void enter(unsigned long now_ms) = 0;
  virtual void exit() = 0;

  virtual void on_mode(unsigned long now_ms) = 0;
  virtual void on_down(unsigned long now_ms) = 0;
  virtual void on_up(unsigned long now_ms) = 0;

  virtual void draw(Grid &m_grid) const = 0;
};

class ClockScreen : public Screen {
public:
  explicit ClockScreen(ClockModel &clock);

  virtual void enter(unsigned long now_ms) override;
  virtual void exit() override;

  virtual void on_mode(unsigned long now_ms) override;
  virtual void on_down(unsigned long now_ms) override;
  virtual void on_up(unsigned long now_ms) override;

  virtual void draw(Grid &) const override;

private:
  ClockModel &m_clock;
};

// This needs an abstract component to read/save the time
class SetTimeScreen : public Animating, public Screen {
public:
  enum class ClockTarget {
    Time,
    Alarm1,
    Alarm2,
  };

  explicit SetTimeScreen(ClockModel &clock, ClockTarget target,
                         const char *header);

  virtual void enter(unsigned long now_ms) override;
  virtual void exit() override;

  virtual void start(unsigned long now_ms) override;
  virtual void age(unsigned long now_ms) override;

  virtual void on_mode(unsigned long now_ms) override;
  virtual void on_down(unsigned long now_ms) override;
  virtual void on_up(unsigned long now_ms) override;

  virtual void draw(Grid &ui_grid) const override;

private:
  enum class EditFocus { hour, minute, second, ampm };

  void load_time();
  void write_time();

  ClockModel &m_clock;
  ClockTarget m_target;
  const char *m_header;

  RepeatingStepAnimation m_blink;
  EditFocus m_focus;

  WrappingInt<uint8_t> m_hour;
  WrappingInt<uint8_t> m_minute;
  WrappingInt<uint8_t> m_second;
  bool m_is_pm;
};

// Placeholder while I work on the warmup alarm bits
class Blank : public Screen {

  virtual void enter(unsigned long) override;
  virtual void exit() override;

  virtual void on_mode(unsigned long) override;
  virtual void on_down(unsigned long) override;
  virtual void on_up(unsigned long) override;

  virtual void draw(Grid &grid) const override;
};

class Controller {
public:
  Controller(ClockModel &clock, Display &display, Keypad &keypad,
             unsigned int redraw_interval, unsigned int clock_refresh_interval);

  void init(unsigned long now_ms);

  void tick(unsigned long now_ms);

private:
  enum class ScreenId { clock, set_clock, set_alarm, set_warmup };

  void process_inputs(unsigned long now_ms);
  void on_set(unsigned long now_ms);

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
  ScreenId m_screen_id;

  ClockScreen m_clock_screen;
  SetTimeScreen m_set_clock_screen;
  SetTimeScreen m_set_alarm_screen;
  Blank m_set_warmup_screen;

  Screen *m_current_screen;
  Animating *m_animates[3];
  // Scratch for the display so we aren't constantly recreating
  Grid m_ui_grid;
};

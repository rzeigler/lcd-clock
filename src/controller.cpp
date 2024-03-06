#include <controller.hpp>

Controller::Controller(ClockModel &clock, Display &display, Keypad &keypad,
                       unsigned int redraw_interval_ms,
                       unsigned int clock_refresh_interval_ms,
                       unsigned int backlight_duration_ms)
    : m_clock(clock), m_display(display), m_keypad(keypad),
      m_redraw_interval(redraw_interval_ms),
      m_clock_interval(clock_refresh_interval_ms),
      m_backlight_flag(backlight_duration_ms) {}

void Controller::Controller::init(unsigned long now_ms) {
  Wire.begin();
  m_clock.init();
  m_display.init();
  m_keypad.init();
}

void Controller::Controller::tick(unsigned long now_ms) {
  m_backlight_flag.age(now_ms);

  process_inputs(now_ms);
  refresh_clock(now_ms);
  draw(now_ms);
}

void Controller::Controller::process_inputs(unsigned long now_ms) {
  m_keypad.poll();
  if (m_keypad.button_pressed()) {
    m_backlight_flag.start(now_ms);
  }
}

void Controller::Controller::refresh_clock(unsigned long now_ms) {
  if (m_clock_interval.should_refresh(now_ms)) {
    m_clock.read_current();
  }
}

static void formatTimeInto(Grid &grid, const Time &time);
static void formatCellInto(Cursor &cursor, uint8_t num, char pad);

void Controller::Controller::draw(unsigned long now_ms) {
  if (m_redraw_interval.should_refresh(now_ms)) {
    formatTimeInto(m_ui_grid, m_clock.current_time());
    m_display.draw(m_ui_grid);

    m_display.setBacklight(m_backlight_flag.get());
  }
}

void formatTimeInto(Grid &grid, const Time &time) {
  Cursor cursor(grid);
  cursor.blank();
  cursor.home();
  cursor.cr();

  cursor.skip(3);

  uint8_t hour = time.hour == 0 ? 12 : time.hour;
  formatCellInto(cursor, hour, ' ');
  cursor.put(':');
  formatCellInto(cursor, time.minute, '0');
  cursor.put(':');
  formatCellInto(cursor, time.second, '0');
  cursor.put(time.is_pm ? "PM" : "AM");
}

void formatCellInto(Cursor &cursor, uint8_t num, char pad) {
  if (num < 10) {
    cursor.put(pad);
  } else {
    cursor.put(num / 10 + '0');
  }
  cursor.put(num % 10 + '0');
}
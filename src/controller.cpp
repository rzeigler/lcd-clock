#include <controller.hpp>

Controller::Controller(ClockModel &clock, Display &display, Keypad &keypad,
                       unsigned int redraw_interval_ms,
                       unsigned int clock_refresh_interval_ms,
                       unsigned int backlight_duration_ms)
    : m_clock_refresh_interval_ms(clock_refresh_interval_ms),
      m_redraw_interval_ms(redraw_interval_ms),
      m_backlight_duration_ms(backlight_duration_ms), m_clock(clock),
      m_display(display), m_keypad(keypad), m_last_redraw_ms(0),
      m_last_clock_refresh_ms(0), m_last_backlight_enable_ms(0),
      m_backlight_enabled(true) {}

void Controller::Controller::init(unsigned long now_ms) {
  Wire.begin();
  m_clock.init();
  m_display.init();
  m_keypad.init();
}

void Controller::Controller::tick(unsigned long now_ms) {
  refresh_clock(now_ms);

  draw(now_ms);
}

// FIXME: Handle wrapping better
void Controller::Controller::refresh_clock(unsigned long now_ms) {
  if (now_ms < m_last_clock_refresh_ms ||
      now_ms > m_last_clock_refresh_ms + m_clock_refresh_interval_ms) {
    m_last_clock_refresh_ms = now_ms;
    m_clock.read_current();
  }
}

static void formatTimeInto(Grid &grid, const Time &time);
static void formatCellInto(Cursor &cursor, uint8_t num, char pad);

void Controller::Controller::draw(unsigned long now_ms) {
  if (now_ms < m_last_clock_refresh_ms ||
      now_ms > m_last_redraw_ms + m_redraw_interval_ms) {
    m_last_redraw_ms = now_ms;

    formatTimeInto(m_ui_grid, m_clock.current_time());
    m_display.draw(m_ui_grid);

    if (m_backlight_enabled &&
        now_ms > m_last_backlight_enable_ms + m_backlight_duration_ms) {
      m_backlight_enabled = false;
      m_display.setBacklight(false);
    }
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
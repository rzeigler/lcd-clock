#include <controller.hpp>

static void formatTimeInto(Grid &grid, const Time &time);
static void formatCellInto(Cursor &cursor, uint8_t num, char pad);

Controller::Controller(ClockModel &clock, Display &display, Keypad &keypad,
                       unsigned int redraw_interval_ms,
                       unsigned int clock_refresh_interval_ms)
    : m_clock(clock), m_display(display), m_keypad(keypad),
      m_redraw_interval(redraw_interval_ms),
      m_clock_interval(clock_refresh_interval_ms), m_backlight_flag(30000),
      m_display_state(Page::display), m_set_clock(2000), m_set_alarm(2000),
      m_animates{&m_backlight_flag, &m_set_clock, &m_set_alarm},
      m_ui_grid(Array<char, 16>(' ')) {}

void Controller::Controller::init(unsigned long now_ms) {
  Wire.begin();
  m_clock.init();
  m_display.init();
  m_keypad.init();
}

void Controller::Controller::tick(unsigned long now_ms) {
  process_inputs(now_ms);
  process_animations(now_ms);
  refresh_clock(now_ms);
  draw(now_ms);
}

void Controller::Controller::process_animations(unsigned long now_ms) {
  for (auto anim : m_animates) {
    anim->age(now_ms);
  }
}

void Controller::Controller::process_inputs(unsigned long now_ms) {
  m_keypad.poll();

  if (m_keypad.set_pressed()) {
    on_set();
  }

  if (m_keypad.button_pressed()) {
    m_backlight_flag.start(now_ms);
  }
}

void Controller::Controller::on_set() {
  switch (m_display_state) {
  case Page::display:
    m_display_state = Page::set_clock;
    m_set_clock.reset(m_clock.current_time());
  case Page::set_clock:
    m_display_state = Page::set_alarm;
    Serial.println("TODO: set_clock");
    break;
  case Page::set_alarm:
    m_display_state = Page::set_warmup;
    Serial.println("TODO: set_alarm");
    break;
  case Page::set_warmup:
    m_display_state = Page::display;
    Serial.println("TODO: set_warmup");
    break;
  default: // display doesn't need exit logic
    break;
  }
}

void Controller::Controller::refresh_clock(unsigned long now_ms) {
  if (m_clock_interval.should_refresh(now_ms)) {
    m_clock.read_current();
  }
}

void Controller::Controller::draw(unsigned long now_ms) {
  if (m_redraw_interval.should_refresh(now_ms)) {
    // We always toggle the backlight
    m_display.setBacklight(m_backlight_flag.get());

    switch (m_display_state) {
    case Page::display:
      formatTimeInto(m_ui_grid, m_clock.current_time());
      m_display.draw(m_ui_grid);
      break;
    default:
      Cursor c(m_ui_grid);
      c.blank();
      m_display.draw(m_ui_grid);
      break;
    }
  }
}

SetTimePage::SetTimePage(unsigned int blink_interval_ms)
    : m_blink(blink_interval_ms), m_focus(EditFocus::hour), m_hour(12, 1, 12),
      m_minute(0, 0, 59), m_second(0, 0, 59), m_is_pm(false) {}

void SetTimePage::SetTimePage::reset(const Time &init_time) {
  m_hour.set(init_time.hour);
  m_minute.set(init_time.minute);
  m_second.set(init_time.second);
  m_is_pm = init_time.is_pm;

  m_focus = EditFocus::hour;
}

void SetTimePage::SetTimePage::start(unsigned long now_ms) {
  m_blink.start(now_ms);
}

void SetTimePage::SetTimePage::age(unsigned long now_ms) {
  m_blink.age(now_ms);
}

void SetTimePage::SetTimePage::on_mode() {
  switch (m_focus) {
  case EditFocus::hour:
    m_focus = EditFocus::minute;
    break;
  case EditFocus::minute:
    m_focus = EditFocus::second;
    break;
  case EditFocus::second:
    m_focus = EditFocus::ampm;
    break;
  case EditFocus::ampm:
    m_focus = EditFocus::hour;
    break;
  }
}

// TODO: Templatize so we can just direct to an editing state instead of this
// big switch?
void SetTimePage::SetTimePage::on_down() {
  switch (m_focus) {
  case EditFocus::hour:
    m_hour.decr();
    break;
  case EditFocus::minute:
    m_minute.decr();
    break;
  case EditFocus::second:
    m_second.decr();
    break;
  case EditFocus::ampm:
    m_is_pm = !m_is_pm;
    break;
  }
}

void SetTimePage::SetTimePage::on_up() {
  switch (m_focus) {
  case EditFocus::hour:
    m_hour.incr();
    break;
  case EditFocus::minute:
    m_minute.incr();
    break;
  case EditFocus::second:
    m_second.incr();
    break;
  case EditFocus::ampm:
    m_is_pm = !m_is_pm;
    break;
  }
}

void SetTimePage::SetTimePage::draw(Grid &ui_grid) const {}

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
#include <controller.hpp>

static void formatCellInto(Cursor &cursor, uint8_t num, char pad);

Controller::Controller(ClockModel &clock, Display &display, Keypad &keypad,
                       unsigned int redraw_interval_ms,
                       unsigned int clock_refresh_interval_ms)
    : m_clock(clock), m_display(display), m_keypad(keypad),
      m_redraw_interval(redraw_interval_ms),
      m_clock_interval(clock_refresh_interval_ms), m_backlight_flag(30000),
      m_screen_id(ScreenId::clock), m_clock_screen(m_clock),
      m_set_clock_screen(m_clock, SetTimeScreen::ClockTarget::Time, "Set Time"),
      m_set_alarm_screen(m_clock, SetTimeScreen::ClockTarget::Alarm1,
                         "Set Alarm"),
      m_current_screen(&m_clock_screen),
      m_animates{&m_backlight_flag, &m_set_clock_screen, &m_set_alarm_screen},
      m_ui_grid(Array<char, 16>(' ')) {}

void Controller::Controller::init(unsigned long now_ms) {
  Wire.begin();
  m_clock.init();
  m_display.init();
  m_keypad.init();
  m_current_screen->enter(now_ms);
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
    on_set(now_ms);
  }

  if (m_keypad.mode_pressed()) {
    m_current_screen->on_mode(now_ms);
  }

  if (m_keypad.down_pressed()) {
    m_current_screen->on_down(now_ms);
  }

  if (m_keypad.up_pressed()) {
    m_current_screen->on_up(now_ms);
  }

  if (m_keypad.button_pressed()) {
    m_backlight_flag.start(now_ms);
  }
}

void Controller::Controller::on_set(unsigned long now_ms) {
  m_current_screen->exit();
  switch (m_screen_id) {
  case ScreenId::clock:
    m_screen_id = ScreenId::set_clock;
    m_current_screen = &m_set_clock_screen;
    break;
  case ScreenId::set_clock:
    m_screen_id = ScreenId::set_alarm;
    m_current_screen = &m_set_alarm_screen;
    break;
  case ScreenId::set_alarm:
    m_screen_id = ScreenId::set_warmup;
    m_current_screen = &m_set_warmup_screen;
    break;
  case ScreenId::set_warmup:
    m_screen_id = ScreenId::clock;
    m_current_screen = &m_clock_screen;
    break;
  }
  m_current_screen->enter(now_ms);
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
    m_current_screen->draw(m_ui_grid);
    m_display.draw(m_ui_grid);
  }
}

ClockScreen::ClockScreen(ClockModel &clock) : m_clock(clock) {}

// This screen doesn't do much interesting
void ClockScreen::ClockScreen::enter(unsigned long) {}
void ClockScreen::ClockScreen::exit() {}
void ClockScreen::ClockScreen::on_mode(unsigned long) {}
void ClockScreen::ClockScreen::on_down(unsigned long) {}
void ClockScreen::ClockScreen::on_up(unsigned long) {}

void ClockScreen::ClockScreen::draw(Grid &grid) const {
  const Time &time = m_clock.current_time();

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

SetTimeScreen::SetTimeScreen(ClockModel &clock, ClockTarget target,
                             const char *header)
    : m_clock(clock), m_target(target), m_header(header), m_blink(1000, true),
      m_focus(EditFocus::hour), m_hour(12, 1, 12), m_minute(0, 0, 59),
      m_second(0, 0, 59) {}

void SetTimeScreen::SetTimeScreen::enter(unsigned long now_ms) {
  start(now_ms);
}

void SetTimeScreen::SetTimeScreen::exit() {
  Serial.println("TODO: Save the time");
}

void SetTimeScreen::SetTimeScreen::start(unsigned long now_ms) {
  m_blink.start(now_ms);
}

void SetTimeScreen::SetTimeScreen::age(unsigned long now_ms) {
  m_blink.age(now_ms);
}

void SetTimeScreen::SetTimeScreen::on_mode(unsigned long now) {
  m_blink.start(now);
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
void SetTimeScreen::SetTimeScreen::on_down(unsigned long now_ms) {
  m_blink.start(now_ms);
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

void SetTimeScreen::SetTimeScreen::on_up(unsigned long now_ms) {
  m_blink.start(now_ms);
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

const char *BLANK_SPACE = "  ";

void SetTimeScreen::SetTimeScreen::draw(Grid &ui_grid) const {
  Cursor cursor(ui_grid);
  cursor.blank();
  cursor.home();

  if (m_header != NULL) {
    int len = max(16, strlen(m_header));
    int padding = (16 - len) / 2;
    cursor.skip(padding);

    cursor.put(m_header);
  }

  cursor.cr();
  cursor.skip(3);
  // For each of these, we want to implement blinking of the focus
  // Therefore, we draw the component if focus is not on the cell or the blink
  // timer is true
  if (m_focus != EditFocus::hour || m_blink.get()) {
    formatCellInto(cursor, m_hour.get(), ' ');
  } else {
    cursor.put(BLANK_SPACE);
  }

  cursor.put(':');

  if (m_focus != EditFocus::minute || m_blink.get()) {
    formatCellInto(cursor, m_minute.get(), '0');
  } else {
    cursor.put(BLANK_SPACE);
  }

  cursor.put(':');

  if (m_focus != EditFocus::second || m_blink.get()) {
    formatCellInto(cursor, m_second.get(), '0');
  } else {
    cursor.put(BLANK_SPACE);
  }

  if (m_focus != EditFocus::ampm || m_blink.get()) {
    cursor.put(m_is_pm ? "PM" : "AM");
  } else {
    cursor.put(BLANK_SPACE);
  }
}

void SetTimeScreen::SetTimeScreen::load_time() {
  switch (m_target) {
  case ClockTarget::Time:
    const Time &current = m_clock.current_time();
    m_hour.set(current.hour);
    m_minute.set(current.minute);
    m_second.set(current.second);
    m_is_pm = current.is_pm;
    break;
  }
}

void Blank::Blank::enter(unsigned long){};
void Blank::Blank::exit(){};
void Blank::Blank::on_mode(unsigned long){};
void Blank::Blank::on_down(unsigned long){};
void Blank::Blank::on_up(unsigned long){};

void Blank::Blank::draw(Grid &grid) const {
  Cursor c(grid);
  c.blank();
}

void formatCellInto(Cursor &cursor, uint8_t num, char pad) {
  if (num < 10) {
    cursor.put(pad);
  } else {
    cursor.put(num / 10 + '0');
  }
  cursor.put(num % 10 + '0');
}
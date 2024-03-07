#include <controller.hpp>

static void format_clock_field_into(Cursor &cursor, uint8_t num, char pad);

Controller::Controller(ClockModel &clock, Display &display, Keypad &keypad,
                       unsigned int redraw_interval_ms,
                       unsigned int clock_refresh_interval_ms)
    : clock_model(clock), display(display), keypad(keypad),
      redraw_interval(redraw_interval_ms),
      refresh_interval(clock_refresh_interval_ms), backlight_flag(30000),
      screen_id(ScreenId::clock), clock_screen(clock_model),
      set_clock_screen(clock_model, SetTimeScreen::ClockTarget::Time,
                       "Set Time"),
      set_alarm_screen(clock_model, SetTimeScreen::ClockTarget::Alarm1,
                       "Set Alarm"),
      current_screen(&clock_screen), alarm(5),
      animations{&backlight_flag, &set_clock_screen, &set_alarm_screen, &alarm},
      ui_grid(Array<char, 16>(' ')) {}

void Controller::init(unsigned long now_ms) {
  Wire.begin();
  alarm.init();
  clock_model.init();
  display.init();
  keypad.init();
  current_screen->enter(now_ms);
}

void Controller::tick(unsigned long now_ms) {
  processAlarm(now_ms);
  processInputs(now_ms);
  processAnimations(now_ms);
  refreshClock(now_ms);
  draw(now_ms);
}

void Controller::processAlarm(unsigned long now_ms) {
  if (clock_model.pollAlarm()) {
    Serial.println("alarm");
    alarm.trigger(now_ms);
  }
}

void Controller::processAnimations(unsigned long now_ms) {
  for (auto anim : animations) {
    anim->age(now_ms);
  }
}

void Controller::processInputs(unsigned long now_ms) {
  keypad.poll();

  if (keypad.set_pressed()) {
    on_set(now_ms);
  }

  if (keypad.mode_pressed()) {
    current_screen->on_mode(now_ms);
  }

  if (keypad.down_pressed()) {
    current_screen->on_down(now_ms);
  }

  if (keypad.up_pressed()) {
    current_screen->on_up(now_ms);
  }

  if (keypad.button_pressed()) {
    backlight_flag.start(now_ms);
  }
}

void Controller::Controller::on_set(unsigned long now_ms) {
  current_screen->exit();
  switch (screen_id) {
  case ScreenId::clock:
    screen_id = ScreenId::set_clock;
    current_screen = &set_clock_screen;
    break;
  case ScreenId::set_clock:
    screen_id = ScreenId::set_alarm;
    current_screen = &set_alarm_screen;
    break;
  case ScreenId::set_alarm:
    screen_id = ScreenId::clock;
    current_screen = &clock_screen;
    break;
  }
  current_screen->enter(now_ms);
}

void Controller::Controller::refreshClock(unsigned long now_ms) {
  if (refresh_interval.should_refresh(now_ms)) {
    clock_model.readCurrent();
  }
}

void Controller::Controller::draw(unsigned long now_ms) {
  if (redraw_interval.should_refresh(now_ms)) {
    // Need drawable abstraction?
    // We always toggle the backlight
    display.setBacklight(backlight_flag.get());
    current_screen->draw(ui_grid);
    display.draw(ui_grid);
    alarm.draw();
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
  const Time &time = m_clock.currentTime();

  Cursor cursor(grid);
  cursor.blank();
  cursor.home();
  cursor.cr();

  cursor.skip(3);

  uint8_t hour = time.hour == 0 ? 12 : time.hour;
  format_clock_field_into(cursor, hour, ' ');
  cursor.put(':');
  format_clock_field_into(cursor, time.minute, '0');
  cursor.put(':');
  format_clock_field_into(cursor, time.second, '0');
  cursor.put(time.is_pm ? "PM" : "AM");
}

SetTimeScreen::SetTimeScreen(ClockModel &clock, ClockTarget target,
                             const char *header)
    : clock_model(clock), m_target(target), m_header(header),
      m_blink(1000, true), m_focus(EditFocus::hour), hour(12, 1, 12),
      minute(0, 0, 59), second(0, 0, 59) {}

void SetTimeScreen::enter(unsigned long now_ms) {
  m_has_modified = false;
  Time time;
  switch (m_target) {
  case ClockTarget::Time:
    time = clock_model.currentTime();
    break;
  case ClockTarget::Alarm1:
    time = clock_model.readAlarm1();
    break;
  }
  hour.set(time.hour);
  minute.set(time.minute);
  second.set(time.second);
  is_pm = time.is_pm;
}

void SetTimeScreen::exit() {
  if (m_has_modified) {
    Serial.println("SetTimeScreen:write");
    write_time();
  }
}

void SetTimeScreen::start(unsigned long now_ms) { m_blink.start(now_ms); }

void SetTimeScreen::age(unsigned long now_ms) { m_blink.age(now_ms); }

void SetTimeScreen::on_mode(unsigned long now) {
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
void SetTimeScreen::on_down(unsigned long now_ms) {
  m_has_modified = true;
  m_blink.start(now_ms);
  switch (m_focus) {
  case EditFocus::hour:
    hour.decr();
    break;
  case EditFocus::minute:
    minute.decr();
    break;
  case EditFocus::second:
    second.decr();
    break;
  case EditFocus::ampm:
    is_pm = !is_pm;
    break;
  }
}

void SetTimeScreen::on_up(unsigned long now_ms) {
  m_has_modified = true;
  m_blink.start(now_ms);
  switch (m_focus) {
  case EditFocus::hour:
    hour.incr();
    break;
  case EditFocus::minute:
    minute.incr();
    break;
  case EditFocus::second:
    second.incr();
    break;
  case EditFocus::ampm:
    is_pm = !is_pm;
    break;
  }
}

const char *BLANK_SPACE = "  ";

void SetTimeScreen::draw(Grid &ui_grid) const {
  Cursor cursor(ui_grid);
  cursor.blank();
  cursor.home();

  if (m_header != NULL) {
    int len = min(16, strlen(m_header));
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
    format_clock_field_into(cursor, hour.get(), ' ');
  } else {
    cursor.put(BLANK_SPACE);
  }

  cursor.put(':');

  if (m_focus != EditFocus::minute || m_blink.get()) {
    format_clock_field_into(cursor, minute.get(), '0');
  } else {
    cursor.put(BLANK_SPACE);
  }

  cursor.put(':');

  if (m_focus != EditFocus::second || m_blink.get()) {
    format_clock_field_into(cursor, second.get(), '0');
  } else {
    cursor.put(BLANK_SPACE);
  }

  if (m_focus != EditFocus::ampm || m_blink.get()) {
    cursor.put(is_pm ? "PM" : "AM");
  } else {
    cursor.put(BLANK_SPACE);
  }
}

void SetTimeScreen::write_time() {
  switch (m_target) {
  case ClockTarget::Time:
    clock_model.setTime(hour.get(), minute.get(), second.get(), is_pm);
    break;
  case ClockTarget::Alarm1:
    clock_model.setAlarm(hour.get(), minute.get(), second.get(), is_pm);
    break;
  }
}

void Blank::enter(unsigned long){};
void Blank::exit(){};
void Blank::on_mode(unsigned long){};
void Blank::on_down(unsigned long){};
void Blank::on_up(unsigned long){};

void Blank::draw(Grid &grid) const {
  Cursor c(grid);
  c.blank();
}

void format_clock_field_into(Cursor &cursor, uint8_t num, char pad) {
  if (num < 10) {
    cursor.put(pad);
  } else {
    cursor.put(num / 10 + '0');
  }
  cursor.put(num % 10 + '0');
}
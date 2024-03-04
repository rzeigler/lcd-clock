#include <Arduino.h>
#include <button.hpp>
#include <clock.hpp>
#include <display.hpp>
#include <util.hpp>

TimeModel timectrl;
Display display;
Button mode_button(12);
Button set_button(11);
Button left_button(10);
Button right_button(9);

void draw_if_outdated();

static void formatTimeInto(Grid &grid, const Time &time);
static void formatCellInto(Cursor &cursor, uint8_t num, char pad);

void setup() {

  Serial.begin(9600);
  while (!Serial) {
  }

  Wire.begin();

  timectrl.init();
  display.init();

  mode_button.init();
  set_button.init();
  left_button.init();
  right_button.init();
}

void loop() {
  static uint32_t count = 0;
  if (mode_button.pressed() || set_button.pressed() || left_button.pressed() ||
      right_button.pressed()) {
    count += 1;
    Serial.print("count = ");
    Serial.println(count);
  }
  draw_if_outdated();
}

void draw_if_outdated() {
  static Grid ui;

  static uint32_t last_draw = 0;
  uint32_t now = millis();
  uint32_t diff = diff_millis(last_draw, now);

  if (diff > 200) {
    last_draw = now;
    timectrl.read_current();
    formatTimeInto(ui, timectrl.current_time());
    display.draw(ui);
  }
}

void formatTimeInto(Grid &grid, const Time &time) {
  Cursor cursor(grid);
  cursor.blank();
  cursor.home();

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
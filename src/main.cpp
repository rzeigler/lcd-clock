#include <Arduino.h>
#include <button.hpp>
#include <clock.hpp>
#include <display.hpp>
#include <util.hpp>

TimeCtrl timectrl;
Display display;
Button mode_button(12);
Button set_button(11);

void draw_if_outdated();
// Handle overflow

void setup() {

  Serial.begin(9600);
  while (!Serial) {
  }

  Wire.begin();

  timectrl.init();
  display.init();

  mode_button.init();
  set_button.init();
}

void loop() {
  static uint32_t count = 0;
  draw_if_outdated();
  if (mode_button.pressed()) {
    count += 1;
    Serial.print("count = ");
    Serial.println(count);
  }
  if (set_button.pressed()) {
    count += 1;
    Serial.print("count = ");
    Serial.println(count);
  }
}

void draw_if_outdated() {

  static uint32_t last_draw = 0;
  uint32_t now = millis();
  uint32_t diff = diff_millis(last_draw, now);

  if (diff > 200) {
    last_draw = now;
    timectrl.read_current();
    display.draw(timectrl.current_time());
  }
}
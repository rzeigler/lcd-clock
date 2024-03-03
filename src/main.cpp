#include <Arduino.h>
#include <button.hpp>
#include <clock.hpp>
#include <display.hpp>
#include <util.hpp>

TimeCtrl timectrl;
Display display;
Button mode_button(12);

void draw_if_outdated();
// Handle overflow

void setup() {
  Wire.begin();

  timectrl.init();
  display.init();
  mode_button.init();

  Serial.begin(9600);
  while (!Serial) {
  }
}

void loop() { draw_if_outdated(); }

void draw_if_outdated() {
  static uint32_t last_draw = 0;
  uint32_t now = millis();
  uint32_t diff = diff_millis(last_draw, now);

  if (diff > 200) {
    last_draw = now;
    timectrl.read_current();
    display.draw(timectrl.current_time());
  }

  if (mode_button.pressed()) {
    Serial.println("pressed");
  }
}
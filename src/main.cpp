#include <Arduino.h>
#include <clock.hpp>
#include <display.hpp>

TimeCtrl timectrl;
Display display;

void draw_if_outdated();
// Handle overflow
inline uint32_t diff_millis(uint32_t before, uint32_t after) {
  if (after < before) {
    return UINT32_MAX - before + after;
  } else {
    return after - before;
  }
}

void setup() {
  Wire.begin();

  timectrl.init();
  display.init();

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
}
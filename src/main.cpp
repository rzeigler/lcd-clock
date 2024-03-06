#include <Arduino.h>
#include <button.hpp>
#include <clock.hpp>
#include <controller.hpp>
#include <display.hpp>
#include <util.hpp>

ClockModel clock_;
Display display;
Keypad keypad(12, 11, 10, 9);

Controller controller(clock_, display, keypad, fps_to_interval(15),
                      fps_to_interval(10), 30000);

void setup() {

  Serial.begin(9600);
  while (!Serial) {
  }

  controller.init(millis());
}

void loop() { controller.tick(millis()); }

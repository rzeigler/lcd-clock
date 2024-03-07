#include <Arduino.h>
#include <led.hpp>

// start in false state so the alarm doesn't light on reset
AlarmLed::AlarmLed(int pin) : pin(pin), animation(5000, false) {}

void AlarmLed::init() { pinMode(pin, OUTPUT); }

void AlarmLed::draw() {
  int state = animation.get() ? HIGH : LOW;
  digitalWrite(pin, state);
}

void AlarmLed::start(unsigned long now_ms) { animation.start(now_ms); }

void AlarmLed::age(unsigned long now_ms) { animation.age(now_ms); }
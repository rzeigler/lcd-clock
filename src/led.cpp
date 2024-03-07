#include <Arduino.h>
#include <led.hpp>

// start in false state so the alarm doesn't light on reset
AlarmLed::AlarmLed(int pin, unsigned long alarm_duration_s)
    : pin(pin), animation(alarm_duration_s * 1000, false) {}

void AlarmLed::init() { pinMode(pin, OUTPUT); }

void AlarmLed::draw() {
  int state = animation.get() ? HIGH : LOW;
  digitalWrite(pin, state);
}

void AlarmLed::start(unsigned long now_ms) { animation.start(now_ms); }

void AlarmLed::age(unsigned long now_ms) { animation.age(now_ms); }
/// Clock related utilities and functions
#pragma once

#include <Arduino.h>
#include <DS3231.h>

// Holder of a Time plus extra flags used for
struct Time {
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;
  bool is_hour12 = false;
  bool is_pm = false;
};

class ClockModel {
public:
  ClockModel();

  void init();

  void readCurrent();

  inline const Time &currentTime() const { return cur_time; };

  void setTime(uint8_t hour, uint8_t minute, uint8_t second, bool is_pm);

  // Read and write alarm1, only supporting daily alarms
  Time readAlarm1();

  void setAlarm(uint8_t hour, uint8_t minute, uint8_t second, bool is_pm);

  bool pollAlarm();

private:
  DS3231 rtc;
  Time cur_time;
};
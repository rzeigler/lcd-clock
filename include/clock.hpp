/// Clock related utilities and functions
#pragma once

#include <Arduino.h>
#include <DS3231-RTC.h>

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

  void read_current();

  inline const Time &current_time() const { return m_time; };

private:
  DS3231 m_rtc;
  Time m_time;
};
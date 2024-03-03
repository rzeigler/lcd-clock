/// Clock related utilities and functions
#pragma once

#include <Arduino.h>
#include <DS3231.h>

// Holder of a Time plus extra flags used for
struct Time {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  bool is_hour12 = true;
  bool is_pm;

  void formatInto(String &result) const;
};

class TimeCtrl {
public:
  TimeCtrl() {}

  void init();

  void read_current();

  inline const Time &current_time() const { return m_time; };

private:
  DS3231 m_rtc;
  Time m_time;
};
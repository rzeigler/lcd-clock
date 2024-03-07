#include <clock.hpp>

ClockModel::ClockModel() : rtc(), cur_time() {}

// Some notes on how the clock seems to work
// If am/pm is unreliable if we set clock mode while hour > 12 before enabling
// If setClockMode(true), and then set it seems to be ok
// 24-hour time is how to set
//

void ClockModel::init() {
  // Make sure the clock mode is always set to 12 hour
  rtc.setClockMode(true);
  rtc.turnOffAlarm(1);
}

void ClockModel::readCurrent() {
  cur_time.hour = rtc.getHour(cur_time.is_hour12, cur_time.is_pm);
  cur_time.minute = rtc.getMinute();
  cur_time.second = rtc.getSecond();
}

void ClockModel::setTime(uint8_t hour, uint8_t minute, uint8_t second,
                         bool is_pm) {
  if (hour == 12 && !is_pm) {
    hour = 0;
  }
  if (is_pm) {
    hour = 12 + hour;
  }
  rtc.setHour(hour);
  rtc.setMinute(minute);
  rtc.setSecond(second);
}

Time ClockModel::readAlarm1() {
  // Don't care, just filling out the parmas
  byte u;
  bool b;

  Time time;

  return time;
}

void ClockModel::setAlarm(uint8_t hour, uint8_t minute, uint8_t second,
                          bool is_pm) {}

bool ClockModel::pollAlarm() { return false; }
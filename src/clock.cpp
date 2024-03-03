#include <clock.hpp>

static void formatCell(String &str, byte count, char pad);

void Time::formatInto(String &result) const {
  result = "";
  if (is_hour12) {
    result += "12";
  } else {
    formatCell(result, hour, ' ');
  }
  result += ":";
  formatCell(result, minute, '0');
  result += ":";
  formatCell(result, second, '0');

  if (is_pm) {
    result += "PM";
  } else {
    result += "AM";
  }
}

void formatCell(String &str, byte count, char pad) {
  if (count < 10) {
    str += pad;
  }
  str += String(count, DEC);
}

void TimeCtrl::init() {}

void TimeCtrl::read_current() {
  m_time.hour = m_rtc.getHour(m_time.is_hour12, m_time.is_pm);
  m_time.minute = m_rtc.getMinute();
  m_time.second = m_rtc.getSecond();
}
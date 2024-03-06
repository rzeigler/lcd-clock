#include <clock.hpp>

ClockModel::ClockModel() : m_rtc(), m_time() {}

// Some notes on how the clock seems to work
// If am/pm is unreliable if we set clock mode while hour > 12 before enabling
// If setClockMode(true), and then set it seems to be ok
// 24-hour time is how to set
//

void ClockModel::init() {
  // Make sure the clock mode is always set to 12 hour
  m_rtc.setClockMode(true);
}

void ClockModel::read_current() {
  m_time.hour = m_rtc.getHour(m_time.is_hour12, m_time.is_pm);
  m_time.minute = m_rtc.getMinute();
  m_time.second = m_rtc.getSecond();
}

void ClockModel::write_time(uint8_t hour, uint8_t minute, uint8_t second,
                            bool is_pm) {
  if (hour == 12 && !is_pm) {
    hour = 0;
  }
  if (is_pm) {
    hour = 12 + hour;
  }
  m_rtc.setHour(hour);
  m_rtc.setMinute(minute);
  m_rtc.setSecond(second);
}
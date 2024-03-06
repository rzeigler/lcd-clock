#include <clock.hpp>

ClockModel::ClockModel() : m_rtc(), m_time() {}

// Some notes on how the clock seems to work
// If am/pm is unreliable if we set clock mode while hour > 12
// If setClockMode(true), then you can force a PM time by setting hour > 12
// despite above It Seems liek this is enough

void ClockModel::init() {
  // Make sure the clock mode is always set to 12 hour
  m_rtc.setClockMode(true);
}

void ClockModel::read_current() {
  m_time.hour = m_rtc.getHour(m_time.is_hour12, m_time.is_pm);
  m_time.minute = m_rtc.getMinute();
  m_time.second = m_rtc.getSecond();
}
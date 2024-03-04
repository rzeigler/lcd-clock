#include <clock.hpp>

void TimeModel::init() {}

void TimeModel::read_current() {
  m_time.hour = m_rtc.getHour(m_time.is_hour12, m_time.is_pm);
  m_time.minute = m_rtc.getMinute();
  m_time.second = m_rtc.getSecond();
}
#include <ui.hpp>
#include <util.hpp>

DecayingFlag::DecayingFlag(bool start_value, unsigned long decay_time_ms)
    : m_current_value(start_value), m_set_time_ms(0),
      m_decay_time_ms(decay_time_ms) {}

bool DecayingFlag::DecayingFlag::get() const { return m_current_value; }

void DecayingFlag::DecayingFlag::set(bool value, unsigned long now_ms) {
  m_current_value = value;
  if (m_current_value) {
    m_set_time_ms = now_ms;
  }
}

void DecayingFlag::DecayingFlag::age(unsigned long now_ms) {
  if (m_current_value) {
    unsigned long elapsed = diff_millis(m_set_time_ms, now_ms);
    if (elapsed > m_decay_time_ms) {
      m_current_value = false;
    }
  }
}
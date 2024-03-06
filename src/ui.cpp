#include <ui.hpp>
#include <util.hpp>

RefreshInterval::RefreshInterval(unsigned long frame_interval_ms)
    : m_last_frame_time_ms(0), m_frame_interval_ms(frame_interval_ms) {}

bool RefreshInterval::RefreshInterval::should_refresh(unsigned long now_ms) {
  if (diff_millis(m_last_frame_time_ms, now_ms) > m_frame_interval_ms) {
    m_last_frame_time_ms = now_ms;
    return true;
  }
  return false;
}

OneShotStepAnimation::OneShotStepAnimation(unsigned long decay_time_ms)
    : m_current_value(false), m_set_time_ms(0), m_decay_time_ms(decay_time_ms) {
}

bool OneShotStepAnimation::OneShotStepAnimation::get() const {
  return m_current_value;
}

void OneShotStepAnimation::OneShotStepAnimation::start(unsigned long now_ms) {
  m_current_value = true;
  if (m_current_value) {
    m_set_time_ms = now_ms;
  }
}

void OneShotStepAnimation::OneShotStepAnimation::age(unsigned long now_ms) {
  if (m_current_value) {
    unsigned long elapsed = diff_millis(m_set_time_ms, now_ms);
    if (elapsed > m_decay_time_ms) {
      m_current_value = false;
    }
  }
}

RepeatingStepAnimation::RepeatingStepAnimation(unsigned long interval_ms,
                                               bool initial)
    : m_initial(initial), m_current_value(initial), m_last_toggle_ms(0),
      m_interval_ms(interval_ms) {}

bool RepeatingStepAnimation::RepeatingStepAnimation::get() const {
  return m_current_value;
}

// Doesn't force a starting state?
void RepeatingStepAnimation::RepeatingStepAnimation::start(
    unsigned long now_ms) {
  m_current_value = m_initial;
  m_last_toggle_ms = now_ms;
}

void RepeatingStepAnimation::RepeatingStepAnimation::set(bool state) {
  m_current_value = state;
}

void RepeatingStepAnimation::RepeatingStepAnimation::age(unsigned long now_ms) {
  if (diff_millis(m_last_toggle_ms, now_ms) > m_interval_ms) {
    m_last_toggle_ms = now_ms;
    m_current_value = !m_current_value;
  }
}
#pragma once

class RefreshInterval {
public:
  explicit RefreshInterval(unsigned long frame_time_ms);

  bool should_refresh(unsigned long now_ms);

private:
  unsigned long m_last_frame_time_ms;
  unsigned long m_frame_interval_ms;
};

/**
 * A boolean that is time aware and decays to flase after a certain amount of
 * time unless refreshed
 */
class OneShotStepAnimation {
public:
  explicit OneShotStepAnimation(unsigned long decay_time_ms);

  bool get() const;
  void start(unsigned long now_ms);
  void age(unsigned long now_ms);

private:
  bool m_current_value;

  // When was the flag last set
  unsigned long m_set_time_ms;
  // What is the maximum m_elapsed_time can be before we revert to false
  const unsigned long m_decay_time_ms;
};
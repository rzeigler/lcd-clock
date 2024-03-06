#pragma once

/**
 * A boolean that is time aware and decays to flase after a certain amount of
 * time unless refreshed
 */
class DecayingFlag {
public:
  DecayingFlag(bool start_value, unsigned long decay_time_ms);

  bool get() const;
  void set(bool value, unsigned long now_ms);
  void age(unsigned long now_ms);

private:
  bool m_current_value;

  // When was the flag last set
  unsigned long m_set_time_ms;
  // What is the maximum m_elapsed_time can be before we revert to false
  const unsigned long m_decay_time_ms;
};
#pragma once

constexpr unsigned long ms_to_s(unsigned long ms) { return ms * 1000; }

class RefreshInterval {
public:
  explicit RefreshInterval(unsigned long frame_time_ms);

  bool should_refresh(unsigned long now_ms);

private:
  unsigned long m_last_frame_time_ms;
  unsigned long m_frame_interval_ms;
};

// Interface for things that animate in some way
// All top-level animations are driven by the controller in the tick function
class Animating {
public:
  // Start/reset the animation to its initial state
  virtual void start(unsigned long now_ms) = 0;
  // Advance time in the simulation
  virtual void age(unsigned long now_ms) = 0;
};

/**
 * A boolean that is time aware and decays to flase after a certain amount of
 * time unless refreshed
 */
class OneShotStepAnimation : public Animating {
public:
  explicit OneShotStepAnimation(unsigned long decay_time_ms,
                                bool initial = false);

  bool get() const;
  virtual void start(unsigned long now_ms) override;
  virtual void age(unsigned long now_ms) override;

private:
  bool m_current_value;

  // When was the flag last set
  unsigned long m_set_time_ms;
  // What is the maximum m_elapsed_time can be before we revert to false
  const unsigned long m_decay_time_ms;
};

class RepeatingStepAnimation : public Animating {
public:
  explicit RepeatingStepAnimation(unsigned long interval_ms,
                                  bool initial = false);

  bool get() const;
  void set(bool state);
  virtual void start(unsigned long now_ms) override;
  virtual void age(unsigned long now_ms) override;

private:
  bool m_initial;
  bool m_current_value;
  unsigned long m_last_toggle_ms;
  unsigned long m_interval_ms;
};
#pragma once

#include <ui.hpp>

class AlarmLed : public Animating {
public:
  AlarmLed(int pin, unsigned long duration_s);

  void init();
  // Friendly name for start
  inline void trigger(unsigned long now_ms) { start(now_ms); }
  void draw();

  virtual void start(unsigned long now_ms) override;
  virtual void age(unsigned long now_ms) override;

private:
  const int pin;
  OneShotStepAnimation animation;
};
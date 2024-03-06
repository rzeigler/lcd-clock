#pragma once

#include <Arduino.h>
#include <stdint.h>

inline uint32_t diff_millis(uint32_t before, uint32_t after) {
  if (after < before) {
    return UINT32_MAX - before + after;
  } else {
    return after - before;
  }
}

[[noreturn]] inline void panic(const char *msg) {
  Serial.println(msg);
  while (true) {
  }
}
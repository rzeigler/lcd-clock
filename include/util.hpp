#pragma once
#include <stdint.h>

inline uint32_t diff_millis(uint32_t before, uint32_t after) {
  if (after < before) {
    return UINT32_MAX - before + after;
  } else {
    return after - before;
  }
}

inline void panic(String msg) {
  while (true) {
    delay(1000);
    Serial.println(msg);
  }
}
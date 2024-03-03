#pragma once
#include <stdint.h>

inline uint32_t diff_millis(uint32_t before, uint32_t after) {
  if (after < before) {
    return UINT32_MAX - before + after;
  } else {
    return after - before;
  }
}
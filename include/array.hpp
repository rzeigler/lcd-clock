#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <util.hpp>

template <typename T, int count> class Array {
public:
  Array() {}

  Array(T init) {
    for (int i = 0; i < count; i++) {
      m_array[i] = init;
    }
  }

  inline T &at(int i) {
    if (i < 0 || i >= count) {
      panic(String("out of bounds: ") + String(i, DEC));
    }
    return m_array[i];
  }

  inline const T &at(int i) const {
    if (i < 0 || i >= count) {
      panic(String("out of bounds: ") + String(i, DEC));
    }
    return m_array[i];
  }

  T &operator[](int i) { return at(i); }

  const T &operator[](int i) const { return at(i); }

  const T *ptr() const { return m_array; }

  T *ptr() { return m_array; }

  constexpr int len() const { return count; }

private:
  T m_array[count];
};
#pragma once

#include <LCD-I2C.h>
#include <array.hpp>
#include <clock.hpp>

using Grid = Array<Array<char, 16>, 2>;

class Cursor {
public:
  Cursor(Grid &grid) : m_row(0), m_col(0), m_grid(grid) {}

  inline void blank() {
    for (int i = 0; i < m_grid.len(); i++) {
      memset(m_grid[i].ptr(), ' ', m_grid[i].len());
    }
  }

  inline void home() {
    m_row = 0;
    m_col = 0;
  }

  inline void cr() {
    m_row += 1;
    m_col = 0;
  }

  inline void set_row(int row) { m_row = row; }

  inline void put(char c) {
    if (m_row < 2 && m_col < 16) {
      m_grid[m_row][m_col] = c;
      m_col++;
    } else {
      Serial.println("Cursor.put overflow");
    }
  }

  inline void put(const char *str) {
    while (*str != '\0') {
      put(*str);
      str++;
    }
  }

  inline void skip(int n) { m_col += n; }

private:
  int m_row;
  int m_col;
  Grid &m_grid;
};

class Display {
public:
  Display();

  void init();

  void draw(const Grid &grid);

  void setBacklight(bool);

private:
  LCD_I2C m_lcd;
};

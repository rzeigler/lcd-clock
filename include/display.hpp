#pragma once

#include <LCD-I2C.h>
#include <clock.hpp>

class Display {
public:
  Display();

  void init();

  void draw(const Time &time);

private:
  LCD_I2C m_lcd;
};

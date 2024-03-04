#include <display.hpp>

Display::Display() : m_lcd(0x27, 16, 2) {}

void Display::init() {
  // Assumes Wire initialized in setup since we are sharing
  m_lcd.begin(false);
  m_lcd.display();
  m_lcd.backlight();
}

void Display::draw(const Grid &grid) {
  m_lcd.home();

  for (int row = 0; row < grid.len(); ++row) {
    m_lcd.setCursor(0, row);
    auto line = grid[row];
    for (int col = 0; col < line.len(); ++col) {
      m_lcd.print(line[col]);
    }
  }
}

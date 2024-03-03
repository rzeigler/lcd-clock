#include <display.hpp>

static void formatTimeInto(String &destination, const Time &time);
static void formatCellInto(String &destination, uint8_t num, char pad);

Display::Display() : m_lcd(0x27, 16, 2) {}

void Display::init() {
  // Assumes Wire initialized in setup since we are sharing
  m_lcd.begin(false);
  m_lcd.display();
  m_lcd.backlight();
}

void Display::draw(const Time &time) {
  static String time_repr;

  formatTimeInto(time_repr, time);
  // Assume that time_repr is 10 chars because (##:##:##(A|P)M)
  // We are on a 16x2 grid so start for even padding
  m_lcd.setCursor(3, 0);
  m_lcd.print(time_repr);
}

void formatTimeInto(String &destination, const Time &time) {
  destination = "";

  uint8_t hour = time.hour == 0 ? 12 : time.hour;
  formatCellInto(destination, hour, ' ');
  destination += ":";
  formatCellInto(destination, time.minute, '0');
  destination += ":";
  formatCellInto(destination, time.second, '0');
  destination += time.is_pm ? "PM" : "AM";
}

void formatCellInto(String &destination, uint8_t num, char pad) {
  if (num < 0) {
    destination += pad;
  }
  destination += String(num, DEC);
}
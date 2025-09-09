#include <Wire.h>
#include <U8g2lib.h>

// U8g2 constructor for SSD1306 128x32 I2C in page-buffer mode
// Only stores 1 page buffer in memory for all displays, (128x32)/8 = 512 bytes
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display1(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display2(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display3(U8G2_R0, U8X8_PIN_NONE);

// TCA9548A helper
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70); // TCA9548A address
  Wire.write(1 << bus);         // select channel
  Wire.endTransmission();
}

void setup() {
  Wire.begin();

  // Initialize display 1 on channel 7
  TCA9548A(7);
  display1.begin();
  display1.clearBuffer();
  display1.setFont(u8g2_font_6x12_tr);
  display1.drawStr(0, 20, "DISPLAY 1");
  display1.sendBuffer();

  // Initialize display 2 on channel 6
  TCA9548A(6);
  display2.begin();
  display2.clearBuffer();
  display2.setFont(u8g2_font_6x12_tr);
  display2.drawStr(0, 20, "DISPLAY 2");
  display2.sendBuffer();

  // Initialize display 3 on channel 5
  TCA9548A(5);
  display3.begin();
  display3.clearBuffer();
  display3.setFont(u8g2_font_6x12_tr);
  display3.drawStr(0, 20, "DISPLAY 3");
  display3.sendBuffer();
}

void loop() {
  // You can update displays the same way:
  // Always select channel first, then update the correct display.
}

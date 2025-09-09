/*
  Display test based on OLED Temperature and Humidity Meter from DroneBot Workshop

  Displays results on two 128 x 32 OLED displays
  Uses TCA9548A I2C Multiplexer
  Uses Adafruit SSD1306 OLED Library
  Uses Adafruit AM2320 Library
  Uses Adafruit GFX Graphics Library
  
  DroneBot Workshop 2020
  https://dronebotworkshop.com
*/
 
// Include Wire Library for I2C
#include <Wire.h>
 
// Include Adafruit Graphics & OLED libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// Create an object for each OLED display
Adafruit_SSD1306 display1(128, 32, &Wire);
Adafruit_SSD1306 display2(128, 32, &Wire);
Adafruit_SSD1306 display3(128, 32, &Wire);

// Use to set channel for I2C communication
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}
 
void setup() {
  // Start Wire library for I2C
  Wire.begin();
  
  // Set multiplexer to channel 7 and initialize OLED-7 with I2C addr 0x3C
  TCA9548A(7);
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  delay(100);

  display1.clearDisplay();
  display1.setTextColor(WHITE);
  display1.setTextSize(2);
  display1.setCursor(0,10);
  display1.print("DISPLAY 1");
  display1.display();
 
  // initialize OLED-6 with I2C addr 0x3C
  TCA9548A(6);
  display2.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  delay(100);

  display2.clearDisplay();
  display2.setTextColor(WHITE);
  display2.setTextSize(2);
  display2.setCursor(0,10);
  display2.print("DISPLAY 2");
  display2.display();

  // initialize OLED-5 with I2C addr 0x3C
  TCA9548A(5);
  display3.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  delay(100);

  display3.clearDisplay();
  display3.setTextColor(WHITE);
  display3.setTextSize(2);
  display3.setCursor(0,10);
  display3.print("DISPLAY 3");
  display3.display();
}

void loop() {

}
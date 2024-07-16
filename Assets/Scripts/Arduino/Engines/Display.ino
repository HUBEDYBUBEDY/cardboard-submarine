#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CHAR_WIDTH 6    // width of chars in pixels + 1 pixel space
#define CHAR_HEIGHT 7   // height of chars in pixels

void displaySetup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setRotation(3);   // Set display to vertical
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void updateDisplay(char* string, byte length) {
  display.clearDisplay();
  display.setCursor(charOffset(length), 0);
  display.println(string);
  // display.writeFastHLine(0, 4, display.width(), SSD1306_WHITE);
  display.display();      // Show initial text
}

byte charOffset(byte numOfChars) {
  return (display.width()/2) - (numOfChars*CHAR_WIDTH)/2;
}
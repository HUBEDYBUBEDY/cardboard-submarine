#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_HEIGHT 128
#define SCREEN_WIDTH 32
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire, OLED_RESET);

#define CHAR_WIDTH 6    // width of chars in pixels + 1 pixel space
#define CHAR_HEIGHT 7   // height of chars in pixels

// Display variables
const byte H_LINE_OFFSET_X = 5;
const byte H_LINE_OFFSET_Y = 10;
const byte H_LINE_WIDTH = SCREEN_WIDTH-(2*H_LINE_OFFSET_X);
const byte H_LINE_VALUE = 5;
const byte Y_PIXEL_VALUE = H_LINE_OFFSET_Y / H_LINE_VALUE;
const byte SPACER = 2;
const byte PRINT_GAP = 25;

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

void updateDisplay(unsigned char* received, byte length) {
  // Calculate value from input
  uint16_t value = 0;
  if (length >= 2) {
    // If full message received
    value = ((received[1] & ~TYPE_MASK) << 7) + received[0];
    // type = received[1]...
  } else if (length == 1) {
    // If lower byte was delimeter
    value = ((received[0] & ~TYPE_MASK) << 7) + DELIMETER;
    // type = received[0]...
  } else {
    // No characters received
    return;
  }

  // Calculate adjustment to nearest line (set current values to nearest line below)
  int remainder = value % H_LINE_VALUE;
  int currentVal = value - remainder;
  int yOffset = (SCREEN_HEIGHT/2) - (remainder*Y_PIXEL_VALUE);

  // Calculate value and position of top line
  for(yOffset; yOffset > 0; yOffset -= H_LINE_OFFSET_Y) {
    if(currentVal - H_LINE_VALUE >= 0) {
      currentVal -= H_LINE_VALUE;
    } else {
      break;
    }
  }

  // Clear previous display buffer
  display.clearDisplay();

  // Draw centre line
  display.writeFastHLine(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SSD1306_WHITE);

  // Calculate next line to print the value
  int nextPrint = 0;
  if(currentVal > 0) {
    nextPrint = currentVal + (PRINT_GAP - (currentVal % PRINT_GAP));
  }
  char message[10];

  // Print horizontal lines and values
  for(yOffset; yOffset <= SCREEN_HEIGHT; yOffset += H_LINE_OFFSET_Y) {
    if(currentVal < nextPrint) {
      // Draw next horizontal line
      display.writeFastHLine(H_LINE_OFFSET_X, yOffset, H_LINE_WIDTH, SSD1306_WHITE);
    }
    else {
      // Calculate text to print based on value
      sprintf(message, "%um", currentVal);
      length = strlen(message);
      byte offsetX = charOffsetX(length);

      // Print text
      display.setCursor(offsetX, charOffsetY(yOffset));
      display.print(message);

      // Draw spacers on either side
      display.writeFastHLine(0,
        yOffset, offsetX-SPACER, SSD1306_WHITE);
      display.writeFastHLine((SCREEN_WIDTH-offsetX)+SPACER,
        yOffset, offsetX-SPACER, SSD1306_WHITE);
      nextPrint = currentVal + PRINT_GAP;
    }

    currentVal += H_LINE_VALUE;
  }

  // Update display
  display.display();
}

byte charOffsetX(byte numOfChars) {
  return (display.width()/2) - (numOfChars*CHAR_WIDTH)/2;
}

byte charOffsetY(byte centre) {
  return (centre - (CHAR_HEIGHT-1)/2);
}
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* Display setup */
#define SCREEN_HEIGHT   128
#define SCREEN_WIDTH    32
#define OLED_RESET      -1
#define SCREEN_ADDRESS  0x3C
Adafruit_SSD1306 display(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire, OLED_RESET);

#define CHAR_WIDTH      6   // Width of chars in pixels + 1 pixel space
#define CHAR_HEIGHT     7   // Height of chars in pixels


/* Display variables */
const byte H_LINE_GAP = 10;       // Pixel gap between each H line
const byte H_LINE_MARGIN = 5;     // Pixel margin on each side of an intermediate H line
const byte H_LINE_WIDTH = SCREEN_WIDTH-(2*H_LINE_MARGIN); // Pixel width of an intermediate H line

const byte H_LINE_VALUE = 5;      // Depth value of each H line
const byte PRINT_GAP = 25;        // Gap between each printed depth value
const byte V_PIXEL_VALUE = H_LINE_GAP / H_LINE_VALUE; // Value of each pixel, measured vertically

const byte NUM_MARGIN = 2;        // Pixel margin on each side of a printed number

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
    // Full message received
    value = ((received[1] & ~TYPE_MASK) << 8) + received[0];

    // TODO: HANDLE TYPE e.g. type = received[1]...
  } else if (length == 1) {
    // Byte 0 is delimeter
    value = ((received[0] & ~TYPE_MASK) << 8) + DELIMETER;

    // TODO: HANDLE TYPE e.g. type = received[0]...
  } else {
    // No characters received
    return;
  }

    // Calculate adjustment to nearest line (set current values to nearest line below)
  int remainder = value % H_LINE_VALUE;
  int currentVal = value - remainder;
  int yOffset = (SCREEN_HEIGHT/2) - (remainder*V_PIXEL_VALUE);

  // Calculate value and position of top line
  for(yOffset; yOffset > 0; yOffset -= H_LINE_GAP) {
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
  for(yOffset; yOffset <= SCREEN_HEIGHT; yOffset += H_LINE_GAP) {
    if(currentVal < nextPrint) {
      // Draw next horizontal line
      display.writeFastHLine(H_LINE_MARGIN, yOffset, H_LINE_WIDTH, SSD1306_WHITE);
    }
    else {
      // Calculate text to print based on value
      sprintf(message, "%um", currentVal);
      length = strlen(message);

      // Print text, centred on H line
      byte offsetX = charOffsetX(length);
      display.setCursor(offsetX, charOffsetY(yOffset));
      display.print(message);

      // Draw H line on both sides of text
      display.writeFastHLine(0,
        yOffset, offsetX-NUM_MARGIN, SSD1306_WHITE);
      display.writeFastHLine((SCREEN_WIDTH-offsetX)+NUM_MARGIN,
        yOffset, offsetX-NUM_MARGIN, SSD1306_WHITE);
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
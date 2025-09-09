#include <Wire.h>
#include <U8g2lib.h>


/* Display setup */

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C depthDisplay(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C bearingDisplay(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C speedDisplay(U8G2_R0, U8X8_PIN_NONE);

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   32
#define CHAR_WIDTH      6
#define CHAR_HEIGHT     8

#define DEPTH_CHANNEL   7
#define BEARING_CHANNEL 6
#define SPEED_CHANNEL   5


/* Depth display variables */

const byte D_H_LINE_GAP = 10;       // Pixel gap between each H line
const byte D_H_LINE_MARGIN = 5;     // Pixel margin on each side of an intermediate H line
const byte D_H_LINE_WIDTH = SCREEN_HEIGHT-(2*D_H_LINE_MARGIN); // Pixel width of an intermediate H line

const byte D_H_LINE_VALUE = 5;      // Depth value of each H line
const byte D_PRINT_GAP = 25;        // Gap between each printed depth value
const byte D_V_PIXEL_VALUE = D_H_LINE_GAP / D_H_LINE_VALUE; // Value of each pixel, measured vertically

const byte D_NUM_MARGIN = 2;        // Pixel margin on each side of a printed number


/* Speed display variables */

const byte S_H_LINE_GAP = 5;        // Pixel gap between each H line
const byte S_H_LINE_INT_WIDTH = 10; // Pixel width of an intermediate H line
const byte S_H_LINE_PRT_WIDTH = 15; // Pixel width of an H line with printed value

const byte S_H_LINE_VALUE = 1;      // Speed value of each H line
const byte S_PRINT_GAP = 5;         // Gap between each printed speed value
const byte S_V_PIXEL_VALUE = S_H_LINE_GAP / S_H_LINE_VALUE; // Value of each pixel, measured vertically

const byte S_NUM_MARGIN = 2;        // Pixel margin on start of a printed number


void displaySetup() {
  Wire.begin();

  // Initialize depth display
  TCA9548A(DEPTH_CHANNEL);
  depthDisplay.begin();
  depthDisplay.clearBuffer();
  depthDisplay.setDisplayRotation(U8G2_R3);     // Set display to vertical
  depthDisplay.setFont(u8g2_font_profont11_tr);

  // Initialize speed display
  TCA9548A(SPEED_CHANNEL);
  depthDisplay.begin();
  depthDisplay.clearBuffer();
  depthDisplay.setDisplayRotation(U8G2_R3);     // Set display to vertical
  depthDisplay.setFont(u8g2_font_profont11_tr);

  resetDisplays();
}


/* TCA9548A helper to select I2C channel */
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70); // TCA9548A address
  Wire.write(1 << bus);         // select channel
  Wire.endTransmission();
}


void updateDisplay(unsigned char* receivedMsg, byte length) {
  // Calculate value from input
  uint16_t value = 0;
  byte type = 0b00000000;

  if (length >= 2) {
    // Full message received
    value = ((receivedMsg[1] & ~TYPE_MASK) << 8) + receivedMsg[0];
    type = receivedMsg[1] & TYPE_MASK;
  } else if (length == 1) {
    // Byte 0 is delimeter
    value = ((receivedMsg[0] & ~TYPE_MASK) << 8) + DELIMETER;
    type = receivedMsg[0] & TYPE_MASK;
  } else {
    // No characters received
    return;
  }

  if (type == TYPE[DEPTH_PIN]) {
    updateDepth(value);
  } else if (type == TYPE[STEER_PIN]) {
    //updateBearing(value);
  } else if (type == TYPE[THRUST_PIN]) {
    updateSpeed(value);
  }
}

void updateDepth(uint16_t value) {
  TCA9548A(DEPTH_CHANNEL);

  // Calculate adjustment to nearest line (set current value to nearest line below in value)
  int remainder = value % D_H_LINE_VALUE;
  int currentLineVal = value - remainder;
  int yOffset = (SCREEN_WIDTH/2) - (remainder*D_V_PIXEL_VALUE);

  // Calculate value and position of top line
  for(yOffset; yOffset > 0; yOffset -= D_H_LINE_GAP) {
    if(currentLineVal - D_H_LINE_VALUE >= 0) {
      currentLineVal -= D_H_LINE_VALUE;
    } else {
      break;
    }
  }
  
  // Clear previous display buffer
  depthDisplay.clearBuffer();

  // Calculate next line to print the value
  int nextPrint = 0;
  if(currentLineVal > 0) {
    nextPrint = currentLineVal + (D_PRINT_GAP - (currentLineVal % D_PRINT_GAP));
  }
  char message[3];

  // Print horizontal lines and values
  for(yOffset; yOffset <= SCREEN_WIDTH; yOffset += D_H_LINE_GAP) {
    if(currentLineVal < nextPrint) {
      // Draw next horizontal line
      depthDisplay.drawHLine(D_H_LINE_MARGIN, yOffset, D_H_LINE_WIDTH);
    }
    else {
      // Calculate text to print based on value
      sprintf(message, "%u", currentLineVal);
      length = strlen(message);

      // Print text, centred on H line
      byte xOffset = charOffsetX(length);
      depthDisplay.drawStr(xOffset, charOffsetY(yOffset), message);

      // Draw H line on both sides of text
      depthDisplay.drawHLine(0,
        yOffset, xOffset-D_NUM_MARGIN);
      depthDisplay.drawHLine((SCREEN_HEIGHT-xOffset)+D_NUM_MARGIN,
        yOffset, xOffset-D_NUM_MARGIN);

      nextPrint = currentLineVal + D_PRINT_GAP;
    }

    currentLineVal += D_H_LINE_VALUE;
  }

  // Draw centre line
  depthDisplay.drawHLine(0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

  // Update display
  depthDisplay.sendBuffer();
}


void updateSpeed(uint16_t value) {
  TCA9548A(SPEED_CHANNEL);

  // Calculate adjustment to nearest line (set current values to nearest line below in value)
  int remainder = value % S_H_LINE_VALUE;
  int currentLineVal = value - remainder;
  int yOffset = (SCREEN_WIDTH/2) + (remainder*S_V_PIXEL_VALUE);

  // Calculate value and position of bottom line
  for(yOffset; yOffset <= SCREEN_WIDTH; yOffset += S_H_LINE_GAP) {
    if(currentLineVal - S_H_LINE_VALUE >= 0) {
      currentLineVal -= S_H_LINE_VALUE;
    } else {
      break;
    }
  }
  
  // Clear previous display buffer
  depthDisplay.clearBuffer();

  // Calculate next line to print the value
  int nextPrint = 0;
  if(currentLineVal > 0) {
    nextPrint = currentLineVal + (S_PRINT_GAP - (currentLineVal % S_PRINT_GAP));
  }
  char message[3];

  // Print horizontal lines and values
  for(yOffset; yOffset > 0; yOffset -= S_H_LINE_GAP) {
    if(currentLineVal < nextPrint) {
      // Draw next horizontal line
      depthDisplay.drawHLine(0, yOffset, S_H_LINE_INT_WIDTH);
    }
    else {
      // Draw H line before text
      depthDisplay.drawHLine(0, yOffset, S_H_LINE_PRT_WIDTH);

      // Calculate text to print based on value
      sprintf(message, "%u", currentLineVal);
      Serial.print(currentLineVal);
      Serial.print(" / ");
      Serial.print(message);
      Serial.println();

      // Print text, centred on H line
      byte xOffset = S_H_LINE_PRT_WIDTH + S_NUM_MARGIN;
      depthDisplay.drawStr(xOffset, charOffsetY(yOffset), message);

      nextPrint = currentLineVal + S_PRINT_GAP;
    }

    currentLineVal += S_H_LINE_VALUE;
  }

  // Draw centre line
  depthDisplay.drawHLine(0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

  // Update display
  depthDisplay.sendBuffer();
}


byte charOffsetX(byte numOfChars) {
  return (SCREEN_HEIGHT/2) - (numOfChars*CHAR_WIDTH)/2;
}


byte charOffsetY(byte centre) {
  return (centre + (CHAR_HEIGHT)/2);
}


void resetDisplays() {
  updateDepth(0);
  // updateBearing(0);
  updateSpeed(0);

  // unsigned char testMsg[2] = {
  //   0b1100100,
  //   TYPE[THRUST_PIN]
  // };
  // updateDisplay(testMsg, 2);
}
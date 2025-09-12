/* Depth display variables */

const uint8_t D_H_LINE_GAP = 10;         // Pixel gap between each H line
const uint8_t D_H_LINE_MARGIN = 5;       // Pixel margin on each side of an intermediate H line
const uint8_t D_H_LINE_WIDTH = SCREEN_HEIGHT-(2*D_H_LINE_MARGIN); // Pixel width of an intermediate H line

const uint8_t D_H_LINE_VALUE = 5;        // Depth value of each H line
const uint8_t D_PRINT_GAP = 25;          // Gap between each printed depth value
const uint8_t D_V_PIXEL_VALUE = D_H_LINE_GAP / D_H_LINE_VALUE; // Value of each pixel, measured vertically

const uint8_t D_NUM_MARGIN = 2;          // Pixel margin on each side of a printed number


/* Bearing display variables */

const uint8_t B_V_LINE_GAP = 5;          // Pixel gap between each V line
const uint8_t B_V_LINE_INT_HEIGHT = 15;  // Pixel height of an intermediate V line
const uint8_t B_V_LINE_PRT_HEIGHT = 20;  // Pixel height of an V line with printed value

const uint8_t B_V_LINE_VALUE = 1;        // Bearing value of each H line
const uint8_t B_PRINT_GAP = 5;           // Gap between each printed bearing value
const uint8_t B_H_PIXEL_VALUE = B_V_LINE_GAP / B_V_LINE_VALUE; // Value of each pixel, measured horizontally

const uint8_t B_NUM_MARGIN = 2;          // Pixel margin on top of a printed number


/* Speed display variables */

const uint8_t S_H_LINE_GAP = 5;          // Pixel gap between each H line
const uint8_t S_H_LINE_INT_WIDTH = 10;   // Pixel width of an intermediate H line
const uint8_t S_H_LINE_PRT_WIDTH = 15;   // Pixel width of an H line with printed value

const uint8_t S_H_LINE_VALUE = 1;        // Speed value of each H line
const uint8_t S_PRINT_GAP = 5;           // Gap between each printed speed value
const uint8_t S_V_PIXEL_VALUE = S_H_LINE_GAP / S_H_LINE_VALUE; // Value of each pixel, measured vertically

const uint8_t S_NUM_MARGIN = 2;          // Pixel margin on start of a printed number

const int8_t S_MAX_VALUE = 30;
const int8_t S_MIN_VALUE = -5;


/* TCA9548A helper to select I2C channel */
void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70); // TCA9548A address
  Wire.write(1 << bus);         // select channel
  Wire.endTransmission();
}


void initialiseDisplays() {
  Wire.begin();

  // Initialize depth display
  TCA9548A(DEPTH_CHANNEL);
  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_profont11_tr);

  // Initialize bearing display
  TCA9548A(BEARING_CHANNEL);
  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_profont11_tr);

  // Initialize speed display
  TCA9548A(SPEED_CHANNEL);
  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_profont11_tr);
}


void updateDepth(uint16_t value) {
  TCA9548A(DEPTH_CHANNEL);
  // Set display to vertical
  display.setDisplayRotation(U8G2_R3);

  // Calculate adjustment to nearest line (set current value to nearest line below in value)
  const int remainder = value % D_H_LINE_VALUE;
  int currentVal = value - remainder;
  int currentY = (SCREEN_WIDTH/2) - (remainder*D_V_PIXEL_VALUE);

  // Calculate value and position of top line
  for (currentY; currentY >= 0; currentY -= D_H_LINE_GAP) {
    if (currentVal - D_H_LINE_VALUE >= 0) {
      currentVal -= D_H_LINE_VALUE;
    } else {
      break;
    }
  }
  
  // Clear previous display buffer
  display.clearBuffer();

  // Calculate next line to print the value
  int nextPrint = 0;
  if (currentVal > 0) {
    nextPrint = currentVal + (D_PRINT_GAP - (currentVal % D_PRINT_GAP));
  }
  char stringVal[5];

  // Print horizontal lines and values
  for (currentY; currentY < SCREEN_WIDTH; currentY += D_H_LINE_GAP) {
    if (currentVal != nextPrint) {
      // Draw next horizontal line
      display.drawHLine(D_H_LINE_MARGIN, currentY, D_H_LINE_WIDTH);
    }
    else {
      // Calculate text to print based on value
      itoa(currentVal, stringVal, 10);
      uint8_t length = strlen(stringVal);

      // Print text, centred on H line
      const uint8_t xOffset = stringCentredHV(length);
      display.drawStr(xOffset, stringCentredV(currentY), stringVal);

      // Draw H line on both sides of text
      display.drawHLine(0,
        currentY, xOffset-D_NUM_MARGIN);
      display.drawHLine((SCREEN_HEIGHT-xOffset)+D_NUM_MARGIN,
        currentY, xOffset-D_NUM_MARGIN);

      nextPrint = currentVal + D_PRINT_GAP;
    }

    currentVal += D_H_LINE_VALUE;
  }

  // Draw centre line
  display.drawHLine(0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

  // Update display
  display.sendBuffer();
}


void updateBearing(uint16_t value) {
  TCA9548A(BEARING_CHANNEL);
  // Set display to horizontal
  display.setDisplayRotation(U8G2_R0);

  // Calculate adjustment to nearest line (set current values to nearest line anticlockwise in value)
  const int remainder = value % B_V_LINE_VALUE;
  int currentVal = value - remainder;
  int currentX = (SCREEN_WIDTH/2) + (remainder*B_H_PIXEL_VALUE);

  // Calculate value and position of furthest anticlockwise line
  for (currentX; currentX >= 0; currentX -= B_V_LINE_GAP) {
    if (currentVal - B_V_LINE_VALUE < 0) {
      // Adjust for bearing below 0
      currentVal += 360;
    }
    
    currentVal -= B_V_LINE_VALUE;
  }
  
  // Clear previous display buffer
  display.clearBuffer();

  // Calculate next line to print the value
  int nextPrint = currentVal + (B_PRINT_GAP - (currentVal % B_PRINT_GAP));
  if (nextPrint >= 360) nextPrint -= 360;
  char stringVal[5];

  // Print vertical lines and values
  for (currentX; currentX < SCREEN_WIDTH; currentX += B_V_LINE_GAP) {
    if (currentVal != nextPrint) {
      // Draw next vertical line
      display.drawVLine(currentX, 0, B_V_LINE_INT_HEIGHT);
    }
    else {
      // Draw V line before text
      display.drawVLine(currentX, 0, B_V_LINE_PRT_HEIGHT);

      // Calculate text to print based on value
      itoa(currentVal, stringVal, 10);
      uint8_t length = strlen(stringVal);

      // Print text, centred on V line
      display.drawStr(stringCentredHH(length, currentX), SCREEN_HEIGHT, stringVal);

      nextPrint = currentVal + B_PRINT_GAP;
      if (nextPrint >= 360) nextPrint -= 360;
    }

    currentVal += B_V_LINE_VALUE;
    if (currentVal >= 360) currentVal -= 360;
  }

  // Draw centre line
  display.drawVLine(SCREEN_WIDTH/2, 0, SCREEN_HEIGHT);

  // Update display
  display.sendBuffer();
}


void updateSpeed(int8_t value) {
  TCA9548A(SPEED_CHANNEL);
  // Set display to vertical
  display.setDisplayRotation(U8G2_R3);

  // Calculate adjustment to nearest line (set current values to nearest line below in value)
  const int remainder = value % S_H_LINE_VALUE;
  int currentVal = value - remainder;
  int currentY = (SCREEN_WIDTH/2) + (remainder*S_V_PIXEL_VALUE);

  // Calculate value and position of bottom line
  for (currentY; currentY < SCREEN_WIDTH; currentY += B_V_LINE_GAP) {
    if (currentVal - S_H_LINE_VALUE >= S_MIN_VALUE) {
      currentVal -= S_H_LINE_VALUE;
    } else {
      break;
    }
  }
  
  // Clear previous display buffer
  display.clearBuffer();

  // Calculate next line to print the value
  int nextPrint = S_MIN_VALUE;
  if (currentVal > S_MIN_VALUE) {
    nextPrint = currentVal + (S_PRINT_GAP - (currentVal % S_PRINT_GAP));
  }
  char stringVal[5];

  // Print horizontal lines and values
  for (currentY; currentY >= 0; currentY -= S_H_LINE_GAP) {
    if (currentVal != nextPrint) {
      // Draw next horizontal line
      display.drawHLine(0, currentY, S_H_LINE_INT_WIDTH);
    }
    else {
      // Draw H line before text
      display.drawHLine(0, currentY, S_H_LINE_PRT_WIDTH);

      // Calculate text to print based on value
      itoa(currentVal, stringVal, 10);

      // Print text, centred on H line
      const uint8_t xOffset = S_H_LINE_PRT_WIDTH + S_NUM_MARGIN;
      display.drawStr(xOffset, stringCentredV(currentY), stringVal);

      nextPrint = currentVal + S_PRINT_GAP;
    }

    currentVal += S_H_LINE_VALUE;
    if (currentVal > S_MAX_VALUE) break;
  }

  // Draw centre line
  display.drawHLine(0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

  // Update display
  display.sendBuffer();
}


/* Return X value to draw string, centred horizontally on vertical screen */
uint8_t stringCentredHV(uint8_t numOfChars) {
  return (SCREEN_HEIGHT/2) - (numOfChars*CHAR_WIDTH)/2;
}


/* Return X value to draw string, centred horizontally on given X value */
uint8_t stringCentredHH(uint8_t numOfChars, uint8_t centre) {
  return centre - (numOfChars*CHAR_WIDTH-1)/2;
}


/* Return Y value to draw string, centred vertically on given Y value */
uint8_t stringCentredV(uint8_t centre) {
  return (centre + (CHAR_HEIGHT)/2);
}

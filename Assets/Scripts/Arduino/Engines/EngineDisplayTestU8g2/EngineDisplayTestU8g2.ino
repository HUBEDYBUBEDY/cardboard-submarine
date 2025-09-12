#include <Wire.h>
#include <U8g2lib.h>


/* Display setup */

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   32
#define CHAR_WIDTH      6               // Char width + 1 pixel space
#define CHAR_HEIGHT     8

#define DEPTH_CHANNEL   7
#define BEARING_CHANNEL 6
#define SPEED_CHANNEL   5


/* Serial Communication*/

#define DELIMETER '\n'
byte receivedMsg[10];
uint8_t length = 0;


void setup() {
  Serial.begin(115200);

  initialiseDisplays();

  updateDepth(100);
  updateBearing(0);
  updateSpeed(-5);
}


void loop() {
  while (Serial.available() > 0) {
    char rc = Serial.read();
    if (rc != DELIMETER) receivedMsg[length++] = rc;
    else {
      // Full message received
      const uint16_t value = atoi(receivedMsg);
      updateDepth(value);
      updateBearing(value);
      updateSpeed(value);
      memset(receivedMsg, 0, sizeof(receivedMsg));
      length = 0;
    }
  }
}


void freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  Serial.println((int)&v - (__brkval ? (int)__brkval : (int)&__heap_start));
}

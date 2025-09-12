void displaySetup() {
  initialiseDisplays();
  resetDisplays();
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
    updateBearing(value);
  } else if (type == TYPE[THRUST_PIN]) {
    updateSpeed(receivedMsg[0]);
  }
}


void resetDisplays() {
  updateDepth(0);
  updateBearing(0);
  updateSpeed(0);

  // unsigned char testMsg[2] = {
  //   0b1100100,
  //   TYPE[THRUST_PIN]
  // };
  // updateDisplay(testMsg, 2);
}
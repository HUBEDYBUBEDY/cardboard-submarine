const char END_MARKER = 127;

unsigned char received[10];
byte length;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
    length = 0;
}

void loop() {
  checkSerial();
}

void checkSerial() {
  // while (Serial.available() > 0) {
  //   unsigned char rc = Serial.read();
  //   if (rc != END_MARKER) received[length++] = rc;
  //   else {
  //     for(int i = 0; i < length; i++) {
  //       Serial.println(received[i]);
  //     }
  //     memset(received, 0, sizeof(received));
  //     length = 0;
  //   }
  // }

  while (Serial.available() > 0) {
    length = Serial.readBytesUntil(END_MARKER, received, 2);
    if(length >= 1) {
      for(int i = 0; i < length; i++) {
        Serial.println(received[i]);
      }
      memset(received, 0, sizeof(received));
      length = 0;
    }
  }
}

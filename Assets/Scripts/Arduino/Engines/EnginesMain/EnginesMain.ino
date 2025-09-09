/* Interrupt Timer */

const short CLOCK_PERIOD = 10;      // in ms
// Counter and compare values (2000 count == 1ms)
const uint16_t t1_load = 0;
const uint16_t t1_comp = 2000*CLOCK_PERIOD;

// Setup timer to interrupt with fixed period
void timerSetup() {
  // Reset Timer1 Control Reg A to default value
  TCCR1A = 0;

  // Set CTC mode (automatically resets Timer1 count to 0 on compare interrupt)
  TCCR1B &= ~(1 << WGM13);  // bitwise XOR
  TCCR1B |= (1 << WGM12);   // bitwise OR

  // Set timer prescaler of 8 (i.e clock select bits to B010)
  TCCR1B &= ~(1 << CS12);
  TCCR1B |= (1 << CS11);
  TCCR1B &= ~(1 << CS10);

  // Reset timer count
  TCNT1 = t1_load;          
  // Load match value into Output Compare Register for Timer1, match A
  OCR1A = t1_comp;          

  // Enable Timer1 compare interrupt (Output Compare Interrupt Enable for Timer1, match A)
  TIMSK1 = (1 << OCIE1A);   

  // Enable global interrupts
  sei();
}


/* Joysticks */

#define DEPTH_PIN 0   // A0
#define STEER_PIN 1   // A1
#define THRUST_PIN 2  // A2

// Max value for joystick potentiometer
const float POTENTIOMETER_MAX = 1023.0;


/* Unity Communication */

#define DELIMETER 0b11111111

// Bits 6-7 show value type: Depth/Steer/Thrust
const byte TYPE_MASK = 0b11000000;
const byte TYPE[] = {
  0b01000000,
  0b10000000,
  0b11000000
};

unsigned char received[10];
byte length = 0;


/*
  Read joystick values at fixed interval
  (run when Timer1 count matches compare register A)
*/
ISR(TIMER1_COMPA_vect) {
  writeValues();
}

void setup() {
  pinMode(DEPTH_PIN, INPUT);
  pinMode(STEER_PIN, INPUT);
  pinMode(THRUST_PIN, INPUT);
  Serial.begin(9600);

  displaySetup();
  timerSetup();
}

void loop() {
  checkSerial();
}

/* Value 0-100 */
int readVal(char pin) {
  return analogRead(pin) * (100 / POTENTIOMETER_MAX);
}

/*
  Write joystick values between 0-100 as 3 byte chunk:
  1) Depth value between 0-100
  2) Steering value between 0-100
  3) Thrust value between 0-100
*/
void writeValues() {
  byte message[] = {readVal(DEPTH_PIN), readVal(STEER_PIN), readVal(THRUST_PIN), DELIMETER};
  Serial.write(message, 4);
}

/*
  Read characters into memory until delimiter, then update display.
*/
void checkSerial() {
  while (Serial.available() > 0) {
    unsigned char rc = Serial.read();
    if (rc != DELIMETER) received[length++] = rc;
    else {
      // Full message received
      updateDisplay(received, length);
      memset(received, 0, sizeof(received));
      length = 0;
    }
  }
}
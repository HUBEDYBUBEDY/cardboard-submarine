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

const char DEPTH_PIN = 0;   // A0
const char STEER_PIN = 1;   // A1
const char THRUST_PIN = 2;  // A2
const float POTENTIOMETER_MAX = 1023.0;

char message[4];
char symbol[3] = {"DST"};

// Run when Timer1 count matches compare register A
ISR(TIMER1_COMPA_vect) {
  printVal(DEPTH_PIN);
  printVal(STEER_PIN);
  printVal(THRUST_PIN);
}

void setup() {
  pinMode(DEPTH_PIN, INPUT);
  pinMode(STEER_PIN, INPUT);
  pinMode(THRUST_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  timerSetup();
}

void loop() {

}

void printVal(char pin) {
  char val = analogRead(pin) * (100 / POTENTIOMETER_MAX);
  sprintf(message, "%c%u", symbol[pin], val);
  Serial.println(message);
}
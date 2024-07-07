#include <Gamer.h>

Gamer gamer;

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

// Sound
const short winNote = 200;
const short loseNote = 100;

// Run when Timer1 count matches compare register A
ISR(TIMER1_COMPA_vect) {
  if (gamer.isPressed(START)) writeInput(true);

}

void setup() {
  gamer.begin();
  writeInput(false);
  timerSetup();
}

void loop() {
  checkSound();
  checkTargeting();
}
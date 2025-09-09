bool isStartPressed;

bool readInput() {
  return isStartPressed;
}

void writeInput(bool newState) {
  noInterrupts();
  isStartPressed = newState;
  interrupts();
}
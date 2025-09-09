const short MIN_DELAY = 500;
const short MAX_DELAY = 2000;
const short HIT_DELAY = 1500;
bool isTargeted;
bool deployed;
char targetingProgress;          // 0-4 to mark progress of tracking enemy (4 means hit)
unsigned long nextTargeting;

const byte hit[8] = {
  B01011011,
  B00010110,
  B10100100,
  B10111110,
  B00111010,
  B10000101,
  B10011111,
  B00101110
};

const byte circles[4][8] = {
  {
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00000000,
  B00000000,
  B00000000
  }, {
  B00000000,
  B00000000,
  B00111100,
  B00100100,
  B00100100,
  B00111100,
  B00000000,
  B00000000
  }, {
  B00000000,
  B01111110,
  B01000010,
  B01000010,
  B01000010,
  B01000010,
  B01111110,
  B00000000
  }, {
  B11111111,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B11111111
  }
};

// Update targeting based on targeting state and if start button is pressed
// Returns returns new input state
void checkTargeting() {
  bool isStartPressed = readInput();

  if(isTargeted) {
    if(!deployed) {
      if(millis() >= nextTargeting) {
        updateTargeting();
      }
      if(isStartPressed) {
        writeInput(false);
        // Deploy countermeasures and check if player dodged or hit
        if(targetingProgress == 4) {
          playerDodge();
        } else {
          playerHit();
        }
      }
    } else if(millis() >= nextTargeting) {
      resetTargeting();
    }
  } else if(isStartPressed) {
    writeInput(false);
    startTargeting();
  }
}

void startTargeting() {
  isTargeted = true;
  deployed = false;
  targetingProgress = 0;
  nextTargeting = millis() + random(MIN_DELAY, MAX_DELAY);
  gamer.clear();
  playTone(winNote);
}

void resetTargeting() {
  isTargeted = false;
  gamer.clear();
}

// Update display to next targeting level, hit if progress >= 4
void updateTargeting() {
  if(targetingProgress < 4) {
    // Shortest delay for last phase
    if(targetingProgress == 3) nextTargeting = millis() + MIN_DELAY;
    else nextTargeting = millis() + random(MIN_DELAY, MAX_DELAY);
    gamer.printImage(circles[targetingProgress++]);
  } else {
    playerHit();
  }
}

void playerDodge() {
  nextTargeting = millis() + HIT_DELAY;
  deployed = true;
  playTone(winNote);
}

void playerHit() {
  nextTargeting = millis() + HIT_DELAY;
  deployed = true;
  gamer.printImage(hit);
  playTone(loseNote);
}
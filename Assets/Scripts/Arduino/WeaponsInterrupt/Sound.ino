const short TONE_DELAY = 100;
bool isPlaying = false;
unsigned long nextSound;

void checkSound() {
  if(isPlaying && millis() >= nextSound) {
    stopTone();
  }
}

void playTone(short note) {
  isPlaying = true;
  nextSound = millis() + TONE_DELAY;
  gamer.playTone(note);
}

void stopTone() {
  isPlaying = false;
  gamer.stopTone();
}
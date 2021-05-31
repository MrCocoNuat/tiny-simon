// ----------     MAIN METHOD     ----------- //

int main() {
  OSCCAL -= 2; //PER-CHIP ADJUSTMENT!!
  DDRB = (1 << DDB1); //pin1 buzzer out, pin2 button DAC in, pin0 3 4 charlieplexed LEDS, pin0 common
  ADMUX = (1 << ADLAR) | (1 << MUX0); //set Vref to Vcc, left adjust result (for 8 bit operation), and input to pin2/pinA1
  ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0); //enable ADC, set prescaler to 8 (1MHz ADC is OK for low precision)
  DIDR0 = (1 << ADC0D) | (1 << ADC2D) | (1 << ADC3D) | (1 << ADC1D) | (1 << AIN1D) |(1 << AIN0D); //disable digital input buffer on all pins, never needed
  PRR = (1 << PRUSI); //disable USI for power saving

  for (;;) {
    int8_t game = buttonInput(false); //blocking until input, and a lot of randomness

    if (game == 3) freq(); //(limited) Audio Frequency Generator
    else simon(game); //simon says (normal, hard, impossible)
  }
}
//OPTIMAL

// ----------     INTERFACE METHODS     ---------- //

int8_t buttonInput(bool wantNone) { //BLOCKING for input. Parameter defines whether blocks no input or input

  int8_t newInput;
  int8_t polledInput;
  uint8_t stability = 0;

  while (stability < 100) {
    ADCSRA |= (1 << ADSC); //start conversion
    while (ADCSRA & (1 << ADSC)); //while ADC is working, block

    uint8_t reading = ADCH >> 3; //read upper 5 bits for some tolerance, but not too much or in-between buttons start to count
    newInput = -(reading) - 1; //if no button was pressed, newInput is -1. Any combination of buttons makes a lower negative

    if (reading == 15) {
      newInput = 3; //button4, 8bit 126
    }
    else if (reading == 8) {
      newInput = 2; //button3, 8bit 66
    }
    else if (reading == 4) {
      newInput = 1; //button2, 8bit 35
    }
    else if (reading == 2) {
      newInput = 0; //button1, 8bit 19
    }

    if ((!wantNone && newInput <= -1) || (wantNone && newInput != -1) || newInput != polledInput) {
      stability = 0;
      polledInput = newInput;
    }
    else stability++;

    randomLite(0, 1); //call random many times during a game, main source of timing randomness
  }

  return polledInput;
}
//NOT OPTIMAL

void ledOutput(int8_t out) { //nonblocking function, simple PORT mapping
  if (out >= 2) { //out is 2 or 3
    DDRB = DDRB & ~(1 << DDB3) | (1 << DDB4) | (1 << DDB0);
    if (out - 2) PORTB = PORTB & ~(1 << PORTB3) & ~(1 << PORTB0) | (1 << PORTB4);
    else PORTB = PORTB & ~(1 << PORTB4) & ~(1 << PORTB3) | (1 << PORTB0);
  }
  else if (out >= 0) { //out is 0 or 1
    DDRB = DDRB & ~(1 << DDB4) | (1 << DDB3) | (1 << DDB0);
    if (out) PORTB = PORTB & ~(1 << PORTB4) & ~(1 << PORTB3) | (1 << PORTB0);
    else PORTB = PORTB & ~(1 << PORTB4) & ~(1 << PORTB0)  | (1 << PORTB3);
  }
  else { //out must be -1
    DDRB &= ~((1 << DDB4) | (1 << DDB3) | (1 << DDB0));
    PORTB &= ~((1 << PORTB4) | (1 << PORTB3) | (1 << PORTB0));
  }
}
//OPTIMAL

void playNote (int8_t noteNumber) { //Thanks, Technoblogy!! //0 is C2, 12 is C3, ... 72 is C8
  const static uint8_t scale[] PROGMEM = {238, 225, 212, 200, 189, 178, 168, 159, 150, 141, 133, 126};

  uint8_t prescaler = 0;
  if (noteNumber + 1) prescaler = 9 - noteNumber / 12; //set correct octave
  DDRB = DDRB & ~(1 << DDB1) | (prescaler != 0) << 1;
  OCR1C = pgm_read_byte_near(scale + (noteNumber % 12)); //set OCR to correct divisor from progmem list
  TCCR1 = (1 << CTC1) | (1 << COM1A0) | prescaler; //enable timer in CTC mode and OC1A pwm output, set prescaler
}
//OPTIMAL

// ----------     ATTINY METHODS     ---------- //

uint8_t randomLite(uint8_t lo, uint8_t hi) { //Returns a random int in [lo,hi). Don't reverse the args! LFSR with period 127
  static uint8_t r = 1; //randomness comes from the many-calls, not the function itself!

  r |= (!(r & (1 << 0)) ^ !(r & (1 << 1)) ^ !(r & (1 << 2)) ^ !(r & (1 << 5))) << 7; //the linear feedback
  r >>= 1; //the shift

  return r % (hi - lo) + lo;
}
//OPTIMAL

// ----------     GAME METHODS     ---------- //

void simon(uint8_t difficulty) { //0,1,2 = normal, hard, impossible
  uint8_t simonSequence[255]; //should not be IOed from flash, as it is written on the fly
  uint8_t score = 1;

  const uint8_t notes[] = {(difficulty == 2) ? randomLite(15, 39) : 24,
                           (difficulty == 2) ? randomLite(17, 41) : 28 - difficulty,
                           (difficulty == 2) ? randomLite(19, 43) : 31,
                           (difficulty == 2) ? randomLite(21, 45) : 36,
                           -1
                          };

  ledOutput(difficulty); //INTRO SEQUENCE
  for (uint8_t i = 0; i < 5; i++) {
    playNote(notes[i]);
    delay(100);
  }
  ledOutput(-1);
  delay(400);

  simonSequence[0] = randomLite(0, 4); //random of 0,1,2,3

  while (score) { //plays until score overflows to 0
    simonSequence[score] = (simonSequence[score - 1] + randomLite(1, 4)) % 4; //no repeats

    if (difficulty && score % (4 - difficulty) == 0) {
      score++;  //hardmode skips every third stage, impossible skips every other. Normal is normal.
      continue;
    }
    if (difficulty == 2 && score > 4) { //impossible mutates the sequence,
      uint8_t mutated = randomLite(2, score - 1); //never touching the first or last one
      uint8_t prev = simonSequence[mutated - 1];
      uint8_t next = simonSequence[mutated + 1];
      if (prev != next) simonSequence[mutated] = (6 - prev - simonSequence[mutated] - next); //and preserving the no repeats rule
    }

    for (uint8_t i = 0; i < score; i++) {
      ledOutput(simonSequence[i]);
      playNote(notes[simonSequence[i]]);
      delay(75);
      playNote(-1);
      delay(2000 / (difficulty * score + 10)); //lower time to memorize sequence makes the game harder!
    }
    ledOutput(-1);
    playNote(-1);

    buttonInput(true); //blocking until no input

    for (uint8_t i = 0; i < score; i++) {
      if (buttonInput(false) == simonSequence[i]) { //blocking until input
        ledOutput(simonSequence[i]);
        playNote(notes[simonSequence[i]]);
        delay(75);
        playNote(-1);
      } else {
        ledOutput(-1); //FAIL SEQUENCE
        playNote(9); delay(400);
        playNote(-1); delay(100);
        return; //lose!
      }
      buttonInput(true); //blocking until no input
    }
    score++;
    delay(500);
  }

  delay(100);
  return; //success, score is too high
}
//PROBABLY NOT OPTIMAL

void freq() { //crappy audio frequency generator. For tuning? Requires OSCCAL for anything close to true frequencies

  ledOutput(3);
  for (uint8_t i = 12; i < 25; i++) {
    playNote(i);
    delay(75);
  }
  ledOutput(-1);
  playNote(-1);
  delay(250);

  uint8_t noteNumber = 24;

  while (true) {
    playNote(noteNumber & 0b01111111); //update the tone
    delay(25);
    ledOutput(-1);
    buttonInput(true); //wait for no input

    noteNumber |= 0b10000000; //highest bit is flag to detect a change of notenumber from the buttons

    int8_t command = buttonInput(false); //blocks until input

    if (command == 0 && noteNumber > 139) noteNumber -= 140; //octave down (128 + 12)
    if (command == 1 && noteNumber > 128) noteNumber -= 129;//note down
    if (command == 2 && noteNumber < 200) noteNumber -= 127; //note up
    if (command == 3 && noteNumber < 189) noteNumber -= 116; //octave up

    if (!(noteNumber & 0b10000000)) ledOutput(command); //should only light up if the input changed something
  }
}
//NOT OPTIMAL

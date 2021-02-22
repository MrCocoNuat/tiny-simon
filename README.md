# tiny-simon
Enhanced Simon Says: difficulty levels and built-in audio frequency
generator

WIP repo

I used the Arduino IDE to upload this to an ATTINY85 using ISP and
Spence Konde's Attinycore.

The compiled binary is 1360B, allowing usage on any AVR down to
an ATTINY25 (sorry ATTINY13 users). However, to get this optimization,
many register fiddling tricks were necessary. As such, this code will
not directly port to a PIC or other microcontroller.

schematic coming soon

This project can be built on a 10x24 PTH protoboard, available straight
from China for cheap from many retailers, including Amazon.

![Front of PCB](assets/front.jpg)
![Back of PCB, mirrored](assets/back.jpg)

### Important assembly and programming notes:

- The 4-button keypad works as a discrete R2R DAC.
To decrease the current flowing through the bias resistors, they are
all in the 100kOhm range, meaning that the resistance across your skin
will completely mess up the readings/buttons if you touch the analog traces.
This is why the picture shows the back completely covered by kapton tape.
Putting the entire board inside an enclosure is also a good idea.

- The OSCCAL register must be programmed PER-CHIP if a high accuracy 
frequency generator is needed. An easy way is to compare the tone generated
to a known accurate audio source, like a PC or smartphone, or maybe you
have a laboratory frequency generator. Lucky you.

- The resistor in series with the piezo buzzer controls its loudness.
Don't set the resistance too low, or the uC will be damaged. There is
no amplifier to boost the signal coming from the uC pin.

### Usage notes:

- Power the board from 3-6V. Any lower, and the lower wavelength LEDs
will not light up. Any higher and the uC might be damaged. A 1S Li-Ion
cell is perfect, which is why my board included a JST PH 2pin connector
to connect it. Mind the polarity!

- When you turn on the circuit (via the switch or other means) nothing
will happen. The uC is waiting for you to press a button before it will
act. The first three buttons start a Simon game, with different
difficulties:

  - The first button starts a normal game. Nothing is special, each round
adds one cue, and the game never speeds up. The button sounds make a major
chord.

  - The second button starts a hard game. Every other round adds two cues
instead of one, and the game speeds up slightly. The button sounds make a
minor chord.
 
  - The third button starts an impossible game. Each round adds two cues,
the game speeds up quickly, and occasionally a previous cue will change
to something else, messing with memorization. The button sounds are random.

- The fourth button starts the audio frequency generator. After a short
fanfare, the buzzer will begin emitting well-tempered C4, or around 262Hz.
The buttons will change the pitch one octave down, one semitone down, one
semitone up, and one octave up respectively, as long as the frequency remains
within the limits (C2-C7). If the frequency did change, i.e. it didn't hit a
limit, the corresponding LED will blink. There is no way to exit except by
resetting the uC. Sorry.

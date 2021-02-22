# tiny-simon
Enhanced Simon Says: difficulty levels and built-in audio generator

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

Important notes:

- The 4-button keypad works as a discrete R2R DAC.
To decrease the current flowing through the bias resistors, they are
all in the 100kOhm range, meaning that the resistance across your skin
will definitely affect the circuit badly if you touch the analog traces.
This is why the picture shows the back completely covered by kapton tape.

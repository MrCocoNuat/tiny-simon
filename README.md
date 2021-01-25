# tiny-simon
Enhanced Simon Says: difficulty levels and built-in audio generator

WIP repo

I used the Arduino IDE to upload this to an ATTINY85 using ISP, but
you could easily gcc and avrdude the C code yourself (writing your
own delay() function) if you are Arduino-phobic.

The compiled binary is 1360B, allowing usage on any AVR down to
an ATTINY25 (sorry ATTINY13 users). However, to get this optimization,
many register fiddling tricks were necessary. As such, this code will
not directly port to a PIC or other microcontroller.


schematic coming soon

pcb layout coming soon

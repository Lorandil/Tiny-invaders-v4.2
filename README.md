# Tiny Invaders v4.2	

Improved version of Daniel C's (Électro L.I.B) Tiny Invaders v3.1


# Introduction 

When I discovered the Tiny Joypad project (https://www.tinyjoypad.com/tinyjoypad_attiny85) I was immediately fascinated by Tiny invaders.
A full arcade game with a scrolling background in only 8kB - completely written in C++.
It was fun to play, but something was missing...

I thought the game would be even more fun with a score display, and perhaps
a high score display. Problem was, the flash memory was already quite low.
I tried nonetheless and failed.

So I decided to free some memory by compressing the graphics. Et voilà:
RLE compressing the graphics gave me about 1kB of flash, enough to add the
decompression code (~160 bytes), a small font (3x5) and (high) score management
(even with EEPROM storage).
Because there were still some bytes left, I added more features ;)


# New Features 

* Added a score display
* Added high score display
* Added high score screen with name entry of the successful player
  (stored bulletproof in EEPROM)
* Added a point values table which is switching with the intro screen every few seconds.
* Added a friendly game over screen with some aliens in it (but no animations yet).
* Added a changing game background between the levels:
    At first I mirrored the image data on decompression, but later I found out that just using
    an x offset doesn't look much different and saves some valuable flash bytes.
* The intro screen switches every few seconds between TinY INVADERS and alien point values
* Added comments
* Reformatted original code (sorry for that!)
* Also some of the original routines needed to be rewritten for a smaller flash memory footprint.


# Credits

I really want to thank Daniel for all the fun this project gave me, this really was
a challenge and I learned a lot about optimizing for size (and speed) on ATtinys
(and about how frustratingly clever the GCC already is...).


# License 
The original code was published under the GPL v3

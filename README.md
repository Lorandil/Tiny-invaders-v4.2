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
* The intro screen switches every few seconds between TinY INVADERS and alien point values
* Added a friendly game over screen with some aliens in it (but no animations yet).
* Added a changing game background between the levels:
    At first I mirrored the image data on decompression, but later I found out that just using
    an x offset doesn't look much different and saves some valuable flash bytes.
* Added comments
* Reformatted original code (sorry for that, Daniel!)
* Also some of the original routines needed to be rewritten for a smaller flash memory footprint.
* Added compile time support for SH1106 displays: Just uncomment 
  //#define _USE_SH1106_
  at the top of Tiny-invaders.ino to get the display correctly centered on SH1106.
  The #define is necessary, because compared to the SSD1306 the SH1106 internally works with 132 bytes 
  per line. By default the centered 128 bytes (2..129) are displayed, causing two missing rows on the
  left and 2 lines garbage on the right. There is a possiblity that the display's manufacturer can choose
  how to connect the controller pins to the actual display.
* Added a sliding animation as a transition between the levels

# Current Size
  8066 bytes (126 bytes left)

# Credits

I really want to thank Daniel from Électro L.I.B for his great work in the TinyJoypad project.
Working on this project gave me a lot of fun, this really was a challenge and I learned a lot 
about optimizing for size (and speed) on ATtinys (and about how frustratingly clever the GCC 
already is...).


# License 
GNU General Public License v3.0

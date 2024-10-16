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

* Added a score and high score display
* Added high score screen with name entry of the successful player
  (the high score is stored bulletproof in EEPROM)
* The intro screen switches every few seconds between TinY INVADERS, the alien point values table
  and the high score.
* Added a friendly game over screen with some aliens in it (but no animations yet).
* Added a changing game background between the levels:
    At first I mirrored the image data on decompression, but later I found out that just using
    an x offset doesn't look much different and saves some valuable flash bytes.
* Added comments to the code
* Reformatted original code (sorry for that, Daniel!)
* Also some of the original routines needed to be rewritten for a smaller flash memory footprint.
* Added compile time support for SH1106 displays: Just uncomment 
  //#define _USE_SH1106_
  at the top of Tiny-invaders.ino to get the display correctly centered on SH1106.
  The #define is necessary, because compared to the SSD1306 the SH1106 internally works with 132 bytes 
  per line. By default the centered 128 bytes (2..129) are displayed, causing two missing rows on the
  left and 2 lines garbage on the right. There is a possibility that the display's manufacturer can choose
  how to connect the controller pins to the actual display.
* Added a sliding animation as a transition between the levels
* Display level number at the beginning of each new level
* If an outer column of the aliens has been eliminated, the remaining aliens will use the free space and
  always patrol between the screen borders. Does this make the game too easy?

# Current Size (when using the highly compatible 'ssd1306xled' version from [https://github.com/tejashwikalptaru/ssd1306xled]):
  8186 bytes (still 6 bytes left, I had to use assembly language for the decompression routine) using the marvelous 
  'ATTinyCore' from Spence Konde (v1.5.2) 
  [https://github.com/SpenceKonde/ATTinyCore]
  
  Please use the settings from ATTinyCore_settings_for_ATtiny85.png to get the size below 8192 bytes ;)

# Credits

I really want to thank Daniel from Électro L.I.B for his great work in the TinyJoypad project.
Working on this project gave me a lot of fun, this really was a challenge and I learned a lot 
about optimizing for size (and speed) on ATtinys (and about how frustratingly clever the GCC 
already is...).


# License 
GNU General Public License v3.0

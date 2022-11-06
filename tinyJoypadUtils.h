#pragma once

#include <Arduino.h>

// required for _delay_us()
#include <ssd1306xled.h>


////////////////////////////////////////////////////////////
// Pin definitions for the joystick and the fire button
#if defined(__AVR_ATtiny85__)
  // don't change these (or you won't support TinyJoypad anymore!)
  #define LEFT_RIGHT_BUTTON A0
  #define UP_DOWN_BUTTON    A3
  #define FIRE_BUTTON        1
  #define SOUND_PIN          4
  #define SOUND_PORT_DDR  DDRB
  #define SOUND_PORT     PORTB
  // disable extended features
  #undef USE_EXTENDED_CHECKS
  #undef USE_SERIAL_PRINT
#else
  #if defined(__AVR_ATmega2560__)
    // Arduino Mega 2560 (and compatible)
    #define LEFT_RIGHT_BUTTON A0
    #define UP_DOWN_BUTTON    A3
    #define FIRE_BUTTON       A1
    #define SOUND_PIN          6
    #define SOUND_PORT_DDR  DDRB
    #define SOUND_PORT     PORTB
  #elif defined(__AVR_ATmega32U4__)
    // Arduino Leonardo (and compatible)
    #define LEFT_RIGHT_BUTTON A0
    #define UP_DOWN_BUTTON    A3
    #define FIRE_BUTTON       A1
    #define SOUND_PIN          6
    #define SOUND_PORT_DDR  DDRD
    #define SOUND_PORT     PORTD
  #elif defined(__AVR_ATmega328P__)
    // Arduino UNO R3 (and compatible)
    #define LEFT_RIGHT_BUTTON A0
    #define UP_DOWN_BUTTON    A3
    #define FIRE_BUTTON       A1
    #define SOUND_PIN          4
    #define SOUND_PORT_DDR  DDRB
    #define SOUND_PORT     PORTB
  #else
    // change these definitions as required
    #define LEFT_RIGHT_BUTTON A0
    #define UP_DOWN_BUTTON    A3
    #define FIRE_BUTTON       A1
    #define SOUND_PIN          4
    #define SOUND_PORT_DDR  DDRB
    #define SOUND_PORT     PORTB
  #endif
  // use extensive checks (not memory restricted by ATtiny85...)
  #define USE_EXTENDED_CHECKS
  #define USE_SERIAL_PRINT
#endif


////////////////////////////////////////////////////////////
// enable the following line to activate SH1106 support 
//#define _USE_SH1106_

////////////////////////////////////////////////////////////
// enable serial screenshot
//#define _ENABLE_SERIAL_SCREENSHOT_
// perform a serial screenshot if this condition is true:
#define _SERIAL_SCREENSHOT_TRIGGER_CONDITION_ ( isDownPressed() )
// uncomment to disable automatic screenshot function
//#define _SERIAL_SCREENSHOT_NO_AUTO_SHOT_


// funciton for initializing the TinyJoypad (ATtiny85) and other microcontrollers
void InitTinyJoypad();

// function for direct joystick readings
bool isLeftPressed();
bool isRightPressed();
bool isUpPressed();
bool isDownPressed();
bool isFirePressed();
void waitUntilButtonsReleased();
void waitUntilButtonsReleased( const uint8_t delay );

// read analog joystick inputs into internal variables
void readAnalogJoystick();
// functions for working on buffered joystick readings
bool wasLeftPressed();
bool wasRightPressed();
bool wasUpPressed();
bool wasDownPressed();
// returns the analog values
uint16_t getAnalogValueX();
uint16_t getAnalogValueY();

void __attribute__ ((noinline)) _variableDelay_us( uint8_t delayValue );
void Sound( const uint8_t freq, const uint8_t dur );

// functions to simplify display handling between ATtiny85 and Ardafruit_SSD1306
void InitDisplay();
void TinyFlip_PrepareDisplayRow( uint8_t y );
void TinyFlip_SendPixels( uint8_t pixels );
void TinyFlip_FinishDisplayRow();
void TinyFlip_DisplayBuffer();

// screenshot functions
void TinyFlip_SerialScreenshot();
void TinyFlip_CheckForSerialScreenshot();

// serial output without clustering the code with #if !defined(__AVR_ATtiny85__)...
void serialPrint( const char *text );
void serialPrintln( const char *text );
void serialPrint( const __FlashStringHelper *text );
void serialPrintln( const __FlashStringHelper *text );
void serialPrint( const uint16_t number );
void serialPrintln( const uint16_t number );
void serialPrint( const int16_t number );
void serialPrintln( const int16_t number );

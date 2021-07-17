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
#else
  // change these definitions as required
  #define LEFT_RIGHT_BUTTON A0
  #define UP_DOWN_BUTTON    A3
  #define FIRE_BUTTON       A1
#endif


////////////////////////////////////////////////////////////
// enable the following line to activate SH1106 support 
//#define _USE_SH1106_

////////////////////////////////////////////////////////////
// enable serial screenshot
#define _ENABLE_SERIAL_SCREENSHOT_
// perform a serial screenshot if this condition is true:
#define _SERIAL_SCREENSHOT_TRIGGER_CONDITION_ ( isDownPressed() )
// uncomment to disable automatic screenshot function
//#define _SERIAL_SCREENSHOT_NO_AUTO_SHOT_


// funciton for initializing the TinyJoypad (ATtiny85) and other microcontrollers
void InitTinyJoypad();

bool isLeftPressed();
bool isRightPressed();
bool isUpPressed();
bool isDownPressed();
bool isFirePressed();
void waitUntilButtonsReleased();
void waitUntilButtonsReleased( const uint8_t delay );
void _variableDelay_us( uint8_t delayValue );
void Sound( const uint8_t freq, const uint8_t dur );

// functions to simplify display handling between ATtiny85 and Ardafruit_SSD1306
void InitDisplay();
void TinyFlip_PrepareDisplayRow( uint8_t y );
void TinyFlip_SendPixels( uint8_t pixels );
void TinyFlip_FinishDisplayRow();
void TinyFlip_DisplayBuffer();
void TinyFlip_SerialScreenshot();
void TinyFlip_CheckForSerialScreenshot();
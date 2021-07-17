#pragma once

#include <Arduino.h>

// required for _delay_us()
#include <ssd1306xled.h>

#if defined(__AVR_ATtiny85__)
  #define LEFT_RIGHT_BUTTON A0
  #define UP_DOWN_BUTTON    A3
  #define FIRE_BUTTON        1
#else
  #define LEFT_RIGHT_BUTTON A0
  #define UP_DOWN_BUTTON    A3
  #define FIRE_BUTTON       A1
#endif

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
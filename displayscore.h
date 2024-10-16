#pragma once

#include <Arduino.h>

typedef uint16_t SCORE_TYPE;

typedef struct {
  SCORE_TYPE score;
  uint8_t    name[3];
  uint8_t    crcFix;
} HISCORE;

// declare 'volatile' otherwise the performance might decrease dramatically
// when using 'uint32_t' (perhaps because the compiler blocks too many registers
// with the score).
volatile static HISCORE _hiScore;

static SCORE_TYPE score;

void resetScore();
void __attribute__ ((noinline)) addScore( SCORE_TYPE points );
void __attribute__ ((noinline)) setScore( SCORE_TYPE points );
SCORE_TYPE __attribute__ ((noinline))getScore();
void __attribute__ ((noinline)) setHighScorePoints( SCORE_TYPE points );
bool __attribute__ ((noinline)) updateHighScorePoints();
SCORE_TYPE __attribute__ ((noinline)) getHighScorePoints();
uint8_t __attribute__ ((noinline)) calcHighScoreCRC();
// Attention: The returned string has a fixed size of 3 bytes 
// and is *not* terminated with zero!
uint8_t __attribute__ ((noinline))*getHighScoreName();

// EEPROM functions
void __attribute__ ((noinline)) initHighScoreStruct( uint16_t gameAddr );
void __attribute__ ((noinline)) readHighScoreFromEEPROM( uint16_t gameAddr );
void __attribute__ ((noinline)) storeHighScoreToEEPROM( uint16_t gameAddr );

void __attribute__ ((noinline)) convertValueToDigits( SCORE_TYPE value, uint8_t *digits );

// display functions (and helpers)
uint8_t displayText( uint8_t x,uint8_t y );
uint8_t displayZoomedText( uint8_t x, uint8_t y );
void __attribute__ ((noinline)) clearTextBuffer();
void printText( uint8_t x, const uint8_t *text, uint8_t textLength );
void __attribute__ ((noinline)) pgm_printText( uint8_t x, const uint8_t *text, uint8_t textLength );
uint8_t *getTextBuffer();

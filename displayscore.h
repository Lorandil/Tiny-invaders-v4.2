#ifndef _DISPLAYSCORE_H_
#define _DISPLAYSCORE_H_

typedef uint16_t SCORE_TYPE;

typedef struct {
  SCORE_TYPE score;
  char     name[3];
  uint8_t  crcFix;
} HISCORE;

// declare 'volatile' otherwise the performance might decrease dramatically
// when using 'uint32_t' (perhaps because the compiler blocks too many registers
// with the score).
volatile static HISCORE _hiScore;

// Video text memory:
// - one line (of 32 characters) in standard mode,
// - three lines (of 16 characters) in zoomed mode
static uint8_t textBuffer[48];
static SCORE_TYPE score;

void resetScore();
void addScore( SCORE_TYPE points );
void setScore( SCORE_TYPE points );
SCORE_TYPE getScore();
void setHighScorePoints( SCORE_TYPE points );
bool updateHighScorePoints();
SCORE_TYPE getHighScorePoints();
uint8_t calcHighScoreCRC();

// EEPROM functions
void initHighScoreStruct( uint16_t gameAddr );
void readHighScoreFromEEPROM( uint16_t gameAddr );
void storeHighScoreToEEPROM( uint16_t gameAddr );

void convertValueToDigits( SCORE_TYPE value, uint8_t *digits );

// display functions (and helpers)
uint8_t displayText( uint8_t x,uint8_t y );
uint8_t displayZoomedText( uint8_t x, uint8_t y );
void clearText();
void printText( uint8_t x, uint8_t *text, uint8_t textLength );
void pgm_printText( uint8_t x, uint8_t *text, uint8_t textLength );
void printByteHex( uint8_t x, uint8_t value );
void hexdumpEEPROM( uint8_t x, uint16_t addr, uint16_t byteCount );
uint8_t *getTextBuffer();

void SerialPrintHighScoreStruct();
#endif

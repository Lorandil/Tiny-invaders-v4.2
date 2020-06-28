#ifndef _DISPLAYSCORE_H_
#define _DISPLAYSCORE_H_

typedef struct {
  uint32_t score;
  char     name[3];
  uint8_t  crc;
} HISCORE;

static HISCORE _hiScore;

// a single line of text
static uint8_t textBuffer[32];
static uint32_t score;

void resetScore();
void addScore( uint32_t points );
void setScore( uint32_t points );
uint16_t getScore();
void setHighScorePoints( uint32_t points );
void updateHighScorePoints();
uint16_t getHighScorePoints();
uint8_t calcHighScoreCRC();

// EEPROM functions
void initHighScoreStruct( uint16_t gameAddr );
void recoverHighScoreFromEEPROM( uint16_t gameAddr );
void storeHighScoreToEEPROM( uint16_t gameAddr );

void convertValueToDigits( uint32_t value, uint8_t *digits );

// display functions (and helpers)
uint8_t displayText( uint8_t x,uint8_t y );
void clearText();
void printText( uint8_t x, uint8_t *text, uint8_t textLength );
uint8_t *getTextBuffer();

#endif

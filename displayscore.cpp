#include <Arduino.h>
#include <avr/pgmspace.h>
#include "displayscore.h"
#include "smallFont.h"
#include <EEPROM.h>

/*--------------------------------------------------------------*/
void resetScore()
{
  score = 0;
}

/*--------------------------------------------------------------*/
void setScore( uint32_t points )
{
  score = points;
}

/*--------------------------------------------------------------*/
uint16_t getScore()
{
  return( score );
}

/*--------------------------------------------------------------*/
void updateHighScorePoints()
{
  if ( score > getHighScorePoints() )
  {
    setHighScorePoints( score );
  }
}

/*--------------------------------------------------------------*/
void setHighScorePoints( uint32_t points )
{
  _hiScore.score = points;
}

/*--------------------------------------------------------------*/
uint16_t getHighScorePoints()
{
  return( _hiScore.score );
}

/*--------------------------------------------------------------*/
// simple checksum by adding all values, not a real CRC ;)
uint8_t calcHighScoreCRC()
{
  uint8_t crc = 42;
  uint8_t *hiScore = (uint8_t *)&_hiScore;
  uint8_t *hiScoreEnd = *hiScore + sizeof( HISCORE ) - 1;
  // sum up all values
  while ( hiScore < hiScoreEnd )
  {
    crc += *hiScore++;
  }

  return( crc );
}

/*--------------------------------------------------------------*/
void initHighScoreStruct( uint16_t gameAddr )
{
  recoverHighScoreFromEEPROM( gameAddr );
  // CRC not valid?
  if ( calcHighScoreCRC() != _hiScore.crc )
  {
    // initialize with some default values
    memset( &_hiScore, sizeof( HISCORE ), 0 );
    _hiScore.crc = calcHighScoreCRC();
    // and store it
    storeHighScoreToEEPROM( gameAddr );
  }
}

/*--------------------------------------------------------------*/
// read HISCORE struct from EEPROM
void recoverHighScoreFromEEPROM( uint16_t gameAddr )
{
  uint8_t *hiScore = (uint8_t *)&_hiScore;
  uint8_t *hiScoreEnd = hiScore + sizeof( HISCORE );

  while ( hiScore < hiScoreEnd )
  {
    *hiScore++ = EEPROM.read( gameAddr++ );
  }
}

/*--------------------------------------------------------------*/
// store HISCORE struct to EEPROM
void storeHighScoreToEEPROM( uint16_t gameAddr )
{
  uint8_t *hiScore = (uint8_t *)&_hiScore;
  uint8_t *hiScoreEnd = hiScore + sizeof( HISCORE );

  _hiScore.crc = calcHighScoreCRC();

  while ( hiScore < hiScoreEnd )
  {
    EEPROM.update( gameAddr++, *hiScore++ );
  }
}

/*--------------------------------------------------------------*/
void addScore( uint32_t points )
{
  score += points;
}

/*--------------------------------------------------------------*/
// converts 'value' to 6 decimal digits
void convertValueToDigits( uint32_t value, uint8_t *digits )
{
  uint8_t digit = '0';
  while ( value >= 100000 )
  {
    digit++;
    value -= 100000;
  }
  *digits++ = digit;

  digit = '0';
  while ( value >= 10000 )
  {
    digit++;
    value -= 10000;
  }
  *digits++ = digit;
  
  digit = '0';
  while ( value >= 1000 )
  {
    digit++;
    value -= 1000;
  }
  *digits++ = digit;

  digit = '0';
  while ( value >= 100 )
  {
    digit++;
    value -= 100;
  }
  *digits++ = digit;

  digit = '0';
  while ( value >= 10 )
  {
    digit++;
    value -= 10;
  }
  *digits++ = digit;
  
  *digits = value + '0';
}

/*--------------------------------------------------------------*/
// display a line of ascii character from the smallFont in the top line
uint8_t displayText( uint8_t x, uint8_t y )
{
  if ( y == 0 )
  {
    // find appropriate character in text array (font width is 4 px)
    uint8_t value = textBuffer[x >> 2];
    // is it a valid character?
    if ( value != 0 )
    {
      // return the column value
      return( pgm_read_byte( charachterFont3x5 + ( ( value - '0' ) << 2 ) + ( x & 0x03) ) );
    }
  }
  return( 0x00 );
}

/*--------------------------------------------------------------*/
void clearText()
{
  memset( textBuffer, 32, 0x00 );
}

/*--------------------------------------------------------------*/
void printText( uint8_t x, uint8_t *text, uint8_t textLength )
{
  memcpy( textBuffer + x, text, textLength );
}

/*--------------------------------------------------------------*/
uint8_t *getTextBuffer()
{
  return( textBuffer );
}

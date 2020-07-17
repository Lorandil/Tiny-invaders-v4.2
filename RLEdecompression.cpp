
#include <Arduino.h>
#include "RLEcompression.h"


/*--------------------------------------------------------------*/
// Input:  pCompressedData       - pointer to compressed data in RAM
//         uncompressedDatar     - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
//         mirrorData            - mirror the data while uncompressing
uint8_t *RLEdecompress( uint8_t *compressedData,
                        uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                        bool     mirrorFlag )
{
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = *compressedData++;

    if ( count & RLE_COMPRESSED_DATA  )
    {
      // prepare special value (just in case)
      uint8_t value = (count & RLE_COMPRESSED_0xFF ) ? 0xff : 0x00;
      // special value?
      if ( !( count & ( RLE_COMPRESSED_0xFF | RLE_COMPRESSED_0x00 ) ) )
      {
        // get stored value
        value = *compressedData++;
      }
      
      // remove special bit
      count &= 0x1f;
      // add one item again
      count++;

      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = value;
      }
    }
    else
    {
      // add one item again
      count++;

      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = *compressedData++;
      }
    }
    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}


/*--------------------------------------------------------------*/
// Input:  compressedData        - pointer to compressed data in PROGMEM
//         uncompressedDatar     - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
//         mirrorFlag            - mirror the data while uncompressing
uint8_t *pgm_RLEdecompress( uint8_t *compressedData,
                            uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                            bool     mirrorFlag )
{
  // mirror data?
  if ( mirrorFlag )
  {
    // set output pointer behind the end of the buffer
    uncompressedData += uncompressedByteCount;
  }
  
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = pgm_read_byte( compressedData++ );

    if ( count & RLE_COMPRESSED_DATA  )
    {
      // prepare special value (just in case)
      uint8_t value = (count & RLE_COMPRESSED_0xFF ) ? 0xff : 0x00;
      // special value?
      if ( !( count & ( RLE_COMPRESSED_0xFF | RLE_COMPRESSED_0x00 ) ) )
      {
        // get stored value
        value = pgm_read_byte( compressedData++ );
      }
      
      // remove special bit
      count &= 0x1f;
      // add one item again
      count++;
      
      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        if ( mirrorFlag )
        {
          *--uncompressedData = value;
        }
        else
        {
          *uncompressedData++ = value;
        }
      }
    }
    else
    {
      // add one item again
      count++;
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        uint8_t value = pgm_read_byte( compressedData++ );
        if ( mirrorFlag )
        {
          *--uncompressedData = value;
        }
        else
        {
          *uncompressedData++ = value;
        }
      }
    }
    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}

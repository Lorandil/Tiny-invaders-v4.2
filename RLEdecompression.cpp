
#include <Arduino.h>
#include "RLEdecompression.h"

/*--------------------------------------------------------------*/
// Input:  compressedData        - pointer to compressed data in PROGMEM
//         uncompressedData      - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
//         mirrorFlag            - mirror the data while uncompressing
#ifdef _RLE_MIRROR_SUPPORT_
uint8_t *pgm_RLEdecompress( const uint8_t *compressedData,
                            uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                            bool     mirrorFlag )
#else
uint8_t *pgm_RLEdecompress( uint8_t *compressedData,
                            uint8_t *uncompressedData, uint16_t uncompressedByteCount )
#endif
{
  #ifdef _RLE_MIRROR_SUPPORT_
    // mirror data?
    if ( mirrorFlag )
    {
      // set output pointer behind the end of the buffer
      uncompressedData += uncompressedByteCount;
    }
  #endif
  
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
      #ifdef _RLE_MIRROR_SUPPORT_
        if ( mirrorFlag )
        {
          *--uncompressedData = value;
        }
        else
      #endif
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
      #ifdef _RLE_MIRROR_SUPPORT_
        if ( mirrorFlag )
        {
          *--uncompressedData = value;
        }
        else
      #endif
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

#if !defined(__AVR_ATtiny85__) && !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega32U4__)
/*--------------------------------------------------------------*/
// Faster routine with max block size of 256 bytes (replaced by assembly routine on AVR)
// Input:  compressedData        - pointer to compressed data in PROGMEM
//         uncompressedData      - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
uint8_t *pgm_RLEdecompress8( const uint8_t *compressedData,
                             uint8_t *uncompressedData,
                             uint8_t  uncompressedByteCount )
{
  do
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
        uint8_t value = pgm_read_byte( compressedData++ );
        *uncompressedData++ = value;
      }
    }
    // remove processed bytes from count
    uncompressedByteCount -= count;
  }
  while ( uncompressedByteCount != 0 );
 
  return( compressedData );
}
#endif

/*--------------------------------------------------------------*/
// Decompressor for standard encoding with up to 32 subsequent values
// Input:  pCompressedData       - pointer to compressed data in RAM
//         uncompressedDatar     - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
uint8_t *RLEdecompress( const uint8_t *compressedData,
                        uint8_t *uncompressedData, uint16_t uncompressedByteCount )
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
// Decompressor for extended encoding with up to 63 subsequent values
// Input:  pCompressedData       - pointer to compressed data in RAM
//         uncompressedDatar     - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
uint8_t *RLEdecompressExtended( const uint8_t *compressedData, 
                                uint8_t *uncompressedData, uint16_t uncompressedByteCount )
{
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = *compressedData++;

    uint8_t encoding = ( count & RLE2_COMPRESSION_MASK );
    // uncompressed data?
    if ( encoding == RLE2_UNCOMPRESSED_DATA )
    {
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = *compressedData++;
      }
    }
    // compressed data
    else
    {
      uint8_t value = ( encoding > RLE2_COMPRESSED_0x00_DATA ) ? 0xFF
                                                                : ( encoding < RLE2_COMPRESSED_0x00_DATA ) ? *compressedData++
                                                                                                           : 0x00;
      // remove encoding bits
      count -= encoding;

      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = value;
      }
    }

    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}


/*--------------------------------------------------------------*/
// Decompressor for standard encoding with up to 32 subsequent values
// Input:  compressedData        - pointer to compressed data in PROGMEM
//         uncompressedDatar     - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
uint8_t *pgm_RLEdecompress( const uint8_t *compressedData,
                            uint8_t *uncompressedData, uint16_t uncompressedByteCount )
{
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
        *uncompressedData++ = pgm_read_byte( compressedData++ );
      }
    }
    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}


/*--------------------------------------------------------------*/
// Decompressor for extended encoding with up to 63 subsequent values
// Input:  pCompressedData       - pointer to compressed data in PROGMEM
//         uncompressedDatar     - output buffer
//         uncompressedByteCount - size of uncompressed data in bytes
uint8_t *pgm_RLEdecompressExtended( const uint8_t *compressedData, 
                                    uint8_t *uncompressedData, uint16_t uncompressedByteCount )
{
  while ( uncompressedByteCount != 0 )
  {
    uint8_t count = pgm_read_byte( compressedData++ );

    uint8_t encoding = ( count & RLE2_COMPRESSION_MASK );
    // uncompressed data?
    if ( encoding == RLE2_UNCOMPRESSED_DATA )
    {
      // copy stored uncompressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = pgm_read_byte( compressedData++ );
      }
    }
    // compressed data
    else
    {
      uint8_t value = ( encoding > RLE2_COMPRESSED_0x00_DATA ) ? 0xFF
                                                                : ( encoding < RLE2_COMPRESSED_0x00_DATA ) ? pgm_read_byte( compressedData++ )
                                                                                                           : 0x00;
      // remove encoding bits
      count -= encoding;

      // uncompress RLE compressed data
      for ( uint8_t n = 0; n < count; n++ )
      {
        *uncompressedData++ = value;
      }
    }

    // remove processed bytes from count
    uncompressedByteCount -= count;
  }

  return( compressedData );
}

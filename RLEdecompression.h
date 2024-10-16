#pragma once

#include <Arduino.h>

  enum class RLECompressionMode
  {
    standard = 0,
    extended = 1
  };

#include "RLEcompressionDefs.h"

#ifdef _RLE_MIRROR_SUPPORT_
  uint8_t *pgm_RLEdecompress( const uint8_t *compressedData,
                              const uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                              bool           mirrorFlag );
#else
  uint8_t *pgm_RLEdecompress( const uint8_t *compressedData,
                              uint8_t *uncompressedData, uint16_t uncompressedByteCount );
  #if defined(__AVR_ATtiny85__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
    extern "C" uint8_t *pgm_RLEdecompress8( const uint8_t *compressedData,
                                            uint8_t *uncompressedData, uint8_t uncompressedByteCount );
    extern "C" uint8_t *pgm_RLEdecompressExt8( const uint8_t *compressedData,
                                               uint8_t *uncompressedData, uint8_t uncompressedByteCount );
  #else
    uint8_t *pgm_RLEdecompress8( const uint8_t *compressedData,
                                 uint8_t *uncompressedData, uint8_t uncompressedByteCount );
    uint8_t *pgm_RLEdecompressExt8( const uint8_t *compressedData,
                                    uint8_t *uncompressedData, uint8_t uncompressedByteCount );
  #endif                              
#endif

uint8_t *RLEdecompress( const uint8_t *compressedData, 
                        uint8_t *uncompressedData, uint16_t uncompressedByteCount );
uint8_t *RLEdecompressExtended( const uint8_t *compressedData, 
                                uint8_t *uncompressedData, uint16_t uncompressedByteCount );
uint8_t *pgm_RLEdecompress( const uint8_t *compressedData,
                            uint8_t *uncompressedData, uint16_t uncompressedByteCount );
uint8_t *RLEdecompressExtended( const uint8_t *compressedData,
                                uint8_t *uncompressedData, uint16_t uncompressedByteCount );

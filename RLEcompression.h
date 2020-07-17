#ifndef _RLE_COMPRESSION_H_
#define _RLE_COMPRESSION_H_

  const uint8_t RLE_COMPRESSED_DATA = 0x80;
  const uint8_t RLE_COMPRESSED_0xFF = 0x40;
  const uint8_t RLE_COMPRESSED_0x00 = 0x20;

  uint16_t RLEcompress( uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                        uint8_t *compressedData, uint16_t compressedBufferSize,
                        uint8_t maxLen = 31, bool outputToSerial = false );
  uint16_t pgm_RLEcompress( uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                            uint8_t *compressedData, uint16_t compressedBufferSize,
                            uint8_t maxLen = 31, bool outputToSerial = false );
  uint8_t *RLEdecompress( uint8_t *compressedData, 
                          uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                          bool     mirrorFlag );
  uint8_t *pgm_RLEdecompress( uint8_t *compressedData,
                              uint8_t *uncompressedData, uint16_t uncompressedByteCount,
                              bool     mirrorFlag );

#endif

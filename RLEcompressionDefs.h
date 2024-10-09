#pragma once

  // standard compression mode
  #define RLE_COMPRESSED_DATA       0x80
  #define RLE_COMPRESSED_0xFF       0x40
  #define RLE_COMPRESSED_0x00       0x20

  // extended compression mode
  #define RLE2_UNCOMPRESSED_DATA    0x00
  #define RLE2_COMPRESSED_0xFF_DATA 0xC0
  #define RLE2_COMPRESSED_0x00_DATA 0x80
  #define RLE2_COMPRESSED_DATA      0x40
  #define RLE2_COMPRESSION_MASK     0xC0 

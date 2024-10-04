#if defined(__AVR_ATtiny85__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)
      .file "RLEdecompression.S"
      .text
      .global pgm_RLEdecompress8

/*-------- GCC registers ---------*/
#define compressedData_H        R25
#define compressedData_L        R24
#define uncompressedData_H      R23
#define uncompressedData_L      R22
#define uncompressedByteCount   R20
/*--------------------------------*/
#define value                   R25
#define count                   R24
#define temp                    R19
/*--------------------------------*/
#define ZERO_REG                 R1

; Parameters in GCC convention
; R0 is free
; R1 := 0
; R18..R27, R30..R31 are free
; T flag is free
; uint8_t *pgm_RLEdecompress( uint8_t *compressedData,        R25:R24
;                             uint8_t *uncompressedData,      R23:R22
;                             uint8_t  uncompressedByteCount  R20
;                           )
;
pgm_RLEdecompress8:
    movw ZL,compressedData_L
    movw XL,uncompressedData_L

RLEdecompress_do:       // do{
       
    lpm   count,Z+      // uint8_t count = pgm_read_byte( compressedData++ );

    sbrs  count,7		    // if ( !( count & RLE_COMPRESSED_DATA ) )
    rjmp  uncompressed_data
compressed_data:
    // prepare special value (just in case)
  	 						        // uint8_t value = (count & RLE_COMPRESSED_0xFF ) ? 0xff : 0x00;
    mov  value,ZERO_REG	// value = 0x00
    sbrc count,6				// count & RLE_COMPRESSED_0xFF
    subi value,1				// value = 0xff
	  mov  temp,count     // copy 'count'
	  andi temp,0x60		  // if ( !( count & ( RLE_COMPRESSED_0xFF | RLE_COMPRESSED_0x00 ) ) )
	  brne decompress_data_start
    lpm  value,Z+				// value = pgm_read_byte( compressedData++ );
decompress_data_start:
    // remove special bits
    andi count,0x1f     // count &= 0x1f;
    inc  count					// count++
    sub  uncompressedByteCount,count //    uncompressedByteCount -= count;
compressed_data_fill:	  // do {
    st   X+,value				//	 *uncompressedData++ = value;
    subi count,1				//   count--
    brne compressed_data_fill   // } while ( count > 0 )
    
    rjmp RLEdecompress_while
	
uncompressed_data:
    inc  count					// count++
    sub  uncompressedByteCount,count //    uncompressedByteCount -= count;
uncompressed_data_fill: // do {
    lpm  value,Z+				//   value = pgm_read_byte( compressedData++ );
    st   X+,value  			//	 *uncompressedData++ = value;
    subi count,1				//   count--
    brne uncompressed_data_fill // } while ( count > 0 )
    
RLEdecompress_while:
	  cpse uncompressedByteCount,ZERO_REG //} while ( uncompressedByteCount != 0 )
	  rjmp RLEdecompress_do

    movw compressedData_L,ZL

	  ret

#endif
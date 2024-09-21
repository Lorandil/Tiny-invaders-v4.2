      .file "RLEdecompression.S"
      .text
      .global pgm_RLEdecompress8

; Parameters in GCC convention
; R0 is free
; R1 := 0
; R18..R27, R30..R31 are free
; T flag is free
; uint8_t *pgm_RLEdecompress( uint8_t *compressedData,        R25:R24
;                             uint8_t *uncompressedData,      R23:R22
             ;							  uint8_t  uncompressedByteCount	R21:R20
;                           )
;
pgm_RLEdecompress8:
	mov  ZH,R25
	mov  ZL,R24

	mov  XH,uncompressedData_H
	mov  XL,uncompressedData_L

RLEdecompress_while:
	//while ( uncompressedByteCount != 0 )
	mov  R0,uncompressedByteCount_L
	or   R0,uncompressedByteCount_H
	breq end

    //uint8_t count = pgm_read_byte( compressedData++ );
	lpm   count,Z+

	sbrs  count,7		// if ( !( count & RLE_COMPRESSED_DATA ) )
	rjmp  uncompressed_data
compressed_data:
    // prepare special value (just in case)
							    //uint8_t value = (count & RLE_COMPRESSED_0xFF ) ? 0xff : 0x00;
	mov  value,R1				// value = 0x00
    sbrc count,6				// !count & RLE_COMPRESSED_0xFF
	subi value,1				// value = 0xff
	sbrc count,5				// !count & RLE_COMPRESSED_0x00
	lpm  value,Z+				// value = pgm_read_byte( compressedData++ );
	// remove special bits
	andi count,0x1f				// count &= 0x1f;
	inc  count					// count++
	sub  uncompressedByteCount_L,count //    uncompressedByteCount -= count;
	sbc  uncompressedByteCount_H,R1
compressed_data_fill:			// do {
	st   X+,value				//	 *uncompressedData++ = value;
	subi count,1				//   count--
	brne compressed_data_fill   // } while ( count > 0 )

	rjmp RLEdecompress_while
	
uncompressed_data:
	inc  count					// count++
	sub  uncompressedByteCount_L,count //    uncompressedByteCount -= count;
	sbc  uncompressedByteCount_H,R1
uncompressed_data_fill:			// do {
	lpm  value,Z+				//   value = pgm_read_byte( compressedData++ );
	st   X+,value  				//	 *uncompressedData++ = value;
	subi count,1				//   count--
	brne uncompressed_data_fill // } while ( count > 0 )

	rjmp RLEdecompress_while

end:
  ret
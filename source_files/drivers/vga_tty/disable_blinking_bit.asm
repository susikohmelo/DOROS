; The last bit in VGA colors can either be intensity, or blinking.
; 
; Intensity lets us use lighter versions of colors, and we want that.
; If its blinking or intensity is up to the specific hardware, so we have to
; make sure to set it ourselves.

; Big thanks to 'FredFrecrison' from 2017 for simplifying the function

global disable_blink

disable_blink:
	; Read I/O Address 0x03DA to reset index/data flip-flop
	mov	dx, 0x03DA
	in	al, dx

	; Write index 0x30 to 0x03C0 to set register index to 0x30
	mov	dx, 0x03C0
	mov	al, 0x30
	out	dx, al

	; Read from 0x03C1 to get register contents
	inc	dx
	in	al, dx

	; Unset Bit 3 to disable Blink
	and	al, 0xF7

	; Write to 0x03C0 to update register with changed value
	dec	dx
	out	dx, al

	ret

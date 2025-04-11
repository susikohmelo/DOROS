
; The structure of the GDT is very ugly - but it's mandatory.
; This one implements a flat memory model.

GDT_start:
	empty_descriptor: ; 	-	8 Byte segment
		dd	0 ; Four 0 bytes
		dd	0 ; Four 0 bytes
	
	code_descriptor:
		dw	0xFFFF ; Limit	-	16	bits
		dw	0 ; Base 		-	16	bits
		dd	0 ; Base 		-	8	bits

		; Present=1		| 1 bit	|	Is segment used?
		; Privilege=0	| 2 bit	|	Segment hierarchy/privs, 0 is highest
		; Type=1		| 1 bit |	Set to 1 if segment is code or data
		; Type flags ------------------
		; Code=1		| 1 bit |	Has code?
		; Conforming=0	| 1 bit |	Can be executed from a lower privelege?
		; Readable=1	| 1 bit |	Able to read constants?
		; Accessed=0	| 1 bit |	This flag is used by the CPU. Always set 0!
		; Other flags -----------------
		; Granularity=1	| 1 bit |	Multiplies limit by 0x1000, ie more space.
		; 32bits =1		| 1 bit |	Use 32 bit memory?
		db	10011010	; Pres|Priv|Type|Typeflags
		db	11001111	; OtherFlags|Last 4 bits of limit
		db	0			; Last 8 bits of base

	data_descriptor:
		dw	0xFFFF ; Limit	-	16	bits
		dw	0 ; Base 		-	16	bits
		dd	0 ; Base 		-	8	bits

		; Present=1		| 1 bit	|	Is segment used?
		; Privilege=0	| 2 bit	|	Segment hierarchy/privs, 0 is highest
		; Type=1		| 1 bit |	Set to 1 if segment is code or data
		; Type flags ------------------
		; Code=0		| 1 bit |	Has code?
		; Direction=0	| 1 bit |	Should segment grow down instead of up?
		; Writable=1	| 1 bit |	Able to write to this segment?
		; Accessed=0	| 1 bit |	This flag is used by the CPU. Always set 0
		; Other flags -----------------
		; Granularity=1	| 1 bit |	Multiplies limit by 0x1000, ie more space.
		; 32bits =1		| 1 bit |	Use 32 bit memory?
		db	10010010	; Pres|Priv|Type|Typeflags
		db	11001111	; OtherFlags|Last 4 bits of limit
		db	0			; Last 8 bits of base
GDT_end:

GDT_descriptor:
	dw	GDT_end - GDT_start - 1 ; Size of the GDT
	dd	GDT_start

; Setting the constants
CODE_SEG equ code_descriptor - GDT_start
DATA_SEG equ data_descriptor - GDT_start

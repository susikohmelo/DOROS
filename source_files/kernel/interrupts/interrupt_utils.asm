;
; This file handles the assembly part of the most basic interrupt functions.
;
; Inline assembly in C is a thing but it can be kind of wonky.
; A fully separate file should be more clear cut and easy to follow.
;

global	enable_interrupts
global	disable_interrupts

global	load_IDT
global	ioport_in
global	ioport_out

; sti = set interrupts
enable_interrupts:
	sti
	ret

; cli = clear interupts
disable_interrupts:
	cli
	ret

; Load first parameter into edx, then use the lidt (load idt) instruction on it
load_IDT:
	mov	edx, [esp + 4]
	lidt	[edx]
	ret

; 1 parameter, the port to read from (16 bits)
;
; Instructions 'in / out' are the i/o memory equivalent of 'mov'
ioport_in:
	mov	edx, [esp + 4]	; Port to read from, 16 bits
	in	al, dx		; Mov value from port to register 'al'
	ret			; return eax (which contains al)

; 2 parameters, the port to read from (16bit) & the value to write (8 bits)
;
; Instructions 'in / out' are the i/o memory equivalent of 'mov'
ioport_out:
	mov	edx, [esp + 4]	; Port to write to, 16 bits
	mov	eax, [esp + 8]	; Value to write to, 8 bits
	out	dx, al		; Mov value from al to the port
	ret			; return eax (which contains 'al)

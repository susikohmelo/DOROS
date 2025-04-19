;
; The main kernel logic is in kernel.c. 
; This is part of the kernel, but not the main file for the kernel.
;
; This file is used by kernel.c to run functions that were easier to write in
; assembly than C.
;
; Inline assembly in C is a thing but it can be really wonky and this to me
; much more clear cut and easy to follow.
;


extern	handle_keyboard_interrupt

global	load_IDT
global	enable_interrupts
global	disable_interrupts

global	keyboard_handler
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

; This is the address that keyboard interrupts will cause to load
keyboard_handler:
	pushad	; store all registers
	cld	; clear direction. This is usually not necessary but why not.
	call	handle_keyboard_interrupt
	popad
	iretd	; Return from interrupt



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


; This function will get called to actually handle the interrupt
extern	handle_keyboard_interrupt
global	recieve_keyboard_interrupt ; this merely receives and calls the handler

; This is the address that keyboard interrupts will cause to load
recieve_keyboard_interrupt:
	pushad	; store all registers
	cld	; clear direction. This is usually not necessary but why not.
	call	handle_keyboard_interrupt
	popad
	iretd	; Return from interrupt


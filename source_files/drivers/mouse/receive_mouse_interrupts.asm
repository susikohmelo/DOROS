
; This function will get called to actually handle the interrupt
extern	handle_mouse_interrupt
global	recieve_mouse_interrupt ; this merely receives and calls the handler

; This is the address that mouse interrupts will cause to load
recieve_mouse_interrupt:
	pushad	; store all registers
	cld	; clear direction. This is usually not necessary but why not.
	call	handle_mouse_interrupt
	popad
	iretd	; Return from interrupt


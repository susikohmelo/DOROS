; This is just a very simple way to link the previous asm bootloader to the
; C based kernel.
;
; TODO
; This is actually largely unnecessary and will likely be removed later

section .text
	[bits 32]
	[extern main]
	call	main
	jmp	$

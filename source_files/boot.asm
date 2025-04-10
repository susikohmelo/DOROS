; A memory offset of 0x7c00 is needed, mostly for the strings
[org 0x7c00]


mov	ah, 0x0e ; TTY print mode
mov	bx, boot_msg
print_boot_msg:
	mov	al, [bx]
	int 0x10
	inc	bx
	cmp	al, 0
	jne	print_boot_msg

mov	ah, 0
int	0x16 ; Wait for input

jmp $

boot_msg:
	db 0xa, 0xd ; \n \r
	db "DOROS bootloader found!"
	db 0xa, 0xd ; \n \r
	db "Press any key to boot into DOROS."
	db 0xa, 0xa, 0xd ; \n \n \r
	db "Waiting for input ...", 0

; x86 recognizes where to boot by looking for 0x55 0xaa between bits 510 to 512
; This adds 0s to memory until it reaches the 510th bit, then we define 55/aa
times 510 - ($ - $$) db 0
db 0x55, 0xaa

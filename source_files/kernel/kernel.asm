; Bootloader is loaded into 0x7C00
org	0x00
bits	16

; /n /r
%define ENDL 0x0D, 0x0A

start:
	mov	si, boot_msg
	call	prints
	jmp	$


; Print a string onto the BIOS screen
; ds:si points to string
prints:
	push	si
	push	ax
	mov	ah, 0x0e
	mov	bh, 0
	.loop:
		lodsb		; Pops a byte out of ds:si into al
		or	al, al	; check for NULL
		jz	.return	; jump if zero flag is set
		int	0x10	; Write interrupt
		jmp	.loop
	.return:
	pop	ax
	pop	si
	ret

boot_msg: db 'You are now in the kernel :D', ENDL, 0

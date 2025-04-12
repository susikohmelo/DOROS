; Bootloader is loaded into 0x7C00
org	0x7C00
bits	16

; /n /r
%define ENDL 0x0D, 0x0A

start:
	jmp main

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

main:
	; Initialize ds/es - depending on the BIOS they may not be set to 0
	mov	ax, 0
	mov	ds, ax
	mov	es, ax

	; Setup stack below our bootloader
	mov	ss, ax
	mov	sp, 0x7C00

	mov	si, boot_msg
	call	prints
	jmp	$

boot_msg: db 'DOROS bootloader found!', ENDL, 0

; x86 recognizes where to boot by looking for 0x55aa between bits 510 to 512
; This adds 0s to memory until it reaches the 510th bit, then we define 0x55aa
times 510 - ($ - $$) db 0
db 0x55, 0xaa

; Bootloader is loaded into 0x7C00
org	0x7C00
bits	16

; /n /r
%define ENDL 0x0D, 0x0A


; ============================ FAT 12 HEADER ==================================
jmp 	short start
nop

; OEM
db	'MSWIN4.1'
dw	512	; Bytes per sector
db	1	; Sectors per cluster
dw	1	; Reserved sectors
db	2	; Number of FATs
dw	0xE0	; Dir entries count
dw	2880	; Sector count
db	0xF0	; Media descriptor (disk type, this is 3.5")
dw	9	; Number of sectors per FAT
dw	18	; Number of sectors per track
dw	2	; Head count
dd	0	; Hidden sector count
dd	0	; Large sector count

db	0	; Drive number
db	0	; Reserved byte
db	0x28	; Signature
db	0x42, 0x42, 0x42, 0x42 ; Volume id. Anything goes here.
db	'Eggman     '	; Volume label, 11 byte str with space char padding
db	'FAT12   '	; Same thing but FAT12 and 8 bytes

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

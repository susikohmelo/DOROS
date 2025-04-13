; Bootloader is loaded into 0x7C00
org	0x7C00
bits	16

; /n /r
%define ENDL 0x0D, 0x0A


; ============================ FAT 12 HEADER ==================================
jmp 	short start
nop

fat_oem:		db	'MSWIN4.1'
fat_bytes_per_sector:	dw	512	; Bytes per sector
fat_sect_per_cluster:	db	1	; Sectors per cluster
fat_reserved_sector:	dw	1	; Reserved sectors
fat_fat_count:		db	2	; Number of FATs
fat_dir_entry_count:	dw	0xE0	; Dir entries count
fat_sector_count:	dw	2880	; Sector count
fat_media_type:		db	0xF0	; Media descriptor (disk type, this is 3.5")
fat_sectors_per_fat:	dw	9	; Number of sectors per FAT
fat_sectors_per_track:	dw	18	; Number of sectors per track
fat_head_count:		dw	2	; Head count
fat_hidden_sectors:	dd	0	; Hidden sector count
fat_large_sectors:	dd	0	; Large sector count

fat_drive_num:		db	0	; Drive number
			db	0	; Reserved byte
fat_signature:		db	0x28	; Signature
fat_volume_id:		db	0x42, 0x42, 0x42, 0x42 ; Volume id. Anything goes here.
fat_volume_label:	db	'Eggman     '	; Volume label, 11 byte str with space char padding
fat_system_id		db	'FAT12   '	; Same thing but FAT12 and 8 bytes

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



; Convert LBA to CHS
; -	Params
;		- ax = LBA
;	Returns
;		- cx [0-5] = sector num
;		- cx [6-15] = cylinder num
;		- dh = head num
lba_to_chs:
	push	ax
	push	dx

	xor	dx, dx	; Set 0
	div	word [fat_sectors_per_track]	; ax = lba / sectors_per_track
						; dx = remainder
	inc	dx
	mov	cx, dx
	
	xor	dx, dx	; Set 0
	div	word [fat_head_count]	; ax = (lba / sect_per_track) / heads
					; dx = remainder
	mov	dh, dl
	mov	ch, al
	shl	ah, 6
	or	cl, ah	

	pop	ax
	mov	dl, al
	pop	ax
	ret



floppy_read_failure:
	mov	si, read_fail_msg
	call	prints
	jmp	$

; Resets disk controller
; -	Params
;		- d1 = drive num
reset_disk:
	pusha
	mov	ah, 0
	int	0x13
	jc	floppy_read_failure
	popa
	ret


; Reads N sectors from disk
; -	Params
;		- ax = LBA
;		- cl = N (128 max)
;		- dl = drive num
;		- es:bx = address to store data
read_disk:
	;	Save modified registers
	push	ax
	push	bx
	push	cx
	push	dx
	push	di

	push	cx
	call	lba_to_chs
	pop	ax

	mov	ah, 0x02
	; Docs recommend to retry floppy reads at least 3 times if they fail
	mov	di, 3

	.try_read:
		pusha
		stc
		int	0x13		; Read
		jnc	.success	; Success if carry flag is 0

		popa
		call	reset_disk	; Reset disk controller

		dec	di
		test	di, di
		jnz	.try_read	; Retry the read if counter is not 0

	.failure:
		jmp	floppy_read_failure
	
	.success:
		popa

		;	Restore modified registers
		pop	di
		pop	dx
		pop	cx
		pop	bx
		pop	ax
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

	mov	[fat_drive_num], dl	; Save the drive number
	mov	ax, 1			; Sector 1 (second sector)
	mov	cl, 1			; 1 sect to read
	mov	bx, 0x7E00		; Buffer to store the read data to
	call	read_disk

	mov	si, boot_msg
	call	prints
	jmp	$


boot_msg: db 'DOROS bootloader found!', ENDL, 0
read_fail_msg: db 'Reading disk failed :<', ENDL, 0

; x86 recognizes where to boot by looking for 0x55aa between bits 510 to 512
; This adds 0s to memory until it reaches the 510th bit, then we define 0x55aa
times 510 - ($ - $$) db 0
db 0x55, 0xaa

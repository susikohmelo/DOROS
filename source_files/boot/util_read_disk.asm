; This file also includes prints util!
; It's necessary for error messages.
; Keep this in mind when including.

; Yes it's a bit annoying to have to include the path but that's NASM
; Do let me know if there is a smarter way to do it!
%include "source_files/boot/util_prints.asm"

read_fail_msg: db 'Reading disk failed :<', ENDL, 0

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

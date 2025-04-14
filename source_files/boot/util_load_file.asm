%define ENDL 0x0D, 0x0A

file_cluster: dw 0
find_file_fail_msg: db 'File not found :<', ENDL, 0
	
; Find and load a file into specified memory address.
;	Parameters
;		- si = filename (11 bytes exact) 
load_file:
	; Get head_count and sectors_per_track
	push	es
	mov	ah, 0x08
	int	0x13
	jc	floppy_read_failure
	pop	es

	and	cl, 0x3F			; Shave top 2 bits
	xor	ch, ch				; Set high bits to 0
	mov	[fat_sectors_per_track], cx	; sector count

	inc	dh
	mov	[fat_head_count], dh	; Head count

	; Get LBA of rootdir
	mov	ax, [fat_sectors_per_fat]	; LBA = reserved + FATs * sectors_per_fat
	mov	bl, [fat_fat_count]
	xor	bh, bh	; 0
	mul	bx	; ax = FATs(ax) * sectors_per_fat(bx)
	add	ax, [fat_reserved_sectors]	; ax is now at root
	push	ax
	; Stack now has ROOTDIR LBA

	; Get size of rootdir (32 * amount_of_entries) / bytes_per_sect
	mov	ax, [fat_sectors_per_fat]
	shl	ax, 5	; *= 32
	xor	dx, dx ; 0
	div	word [fat_bytes_per_sector]	; Sectors to read

	test	dx, dx	; dx containes the remainder
	jz	.rootdir_calculated
	inc	ax	; round up result if there is a remainder

.rootdir_calculated:

	; Read the root directory
	mov	cl, al			; cl = sectors to read
	pop	ax			; rootdir LBA
	mov	dl, [fat_drive_num]	; drive to read
	mov	bx, buffer
	call	read_disk

	xor	bx, bx ; 0
	mov	di, buffer
	
	; Si contains the string to search for. Si will be modified soon, so
	; ax is used as a temporary storage
	mov	ax, si

.find_file:
	mov	si, ax ; Ax contains the file name
	mov	cx, 11			; memcmp length
	push	di	; save di, as we will modify it and this will loop a lot

	; compares si and di, increments them
	; stops if cx reaches 0. also decreases cx with each iteration
	; Basically this is the x86 asm equivalent of doing this in C:
	;	while (si[i] == di[i] && cx-- != 0)
	;		++ i
	repe	cmpsb
	pop	di
	je	.found_file ; jump if equal

	add	di, 32	; 32 is the size of a directory entry
	inc	bx
	cmp	bx, [fat_dir_entry_count] ; Check that we are below the amount
	jl	.find_file

	.not_found_file:
		jmp	find_file_failed

.found_file:
	; di is still pointing to the dir entry
	mov	ax, [di + 26]	; 26th bit onward is the low 16b cluster entry
				; ie. the first cluster entry
	mov	[file_cluster], ax

	; load FAT
	mov	ax, [fat_reserved_sectors]
	mov	cl, [fat_sectors_per_fat]
	mov	bx, buffer
	call	read_disk

	mov	bx, KERNEL_SEGMENT
	mov	es, bx
	mov	bx, KERNEL_OFFSET

.load_file_loop:

	; first cluster = (file_cluster - 2) * sectors_per_cluster + start_sector
	mov	ax, [file_cluster]
	add	ax, 31			; TODO hardcoded fix later!!!!!!

	mov	cl, 1
	mov	dl, [fat_drive_num]
	call	read_disk

	add	bx, [fat_bytes_per_sector] ; TODO will overflow if file > 64kb

	; Get location of next cluster to read
	; cluster * 3 / 2
	mov	ax, [file_cluster]
	mov	cx, 3
	mul	cx
	mov	cx, 2
	div	cx

	mov	si, buffer
	add	si, ax
	mov	ax, [ds:si]	; next entry

	or	dx, dx
	jz	.even
.odd:
	shr	ax, 4
	jmp	.next_cluster

.even:
	and	ax, 0x0FFF

.next_cluster:
	cmp	ax, 0x0FF8 ; End marker
	jae	.load_file_success

	; Not there yet :<
	mov	[file_cluster], ax
	jmp	.load_file_loop


.load_file_success:
	ret

find_file_failed:
	mov	si, find_file_fail_msg
	call	prints
	jmp	$

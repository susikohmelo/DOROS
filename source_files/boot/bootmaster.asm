; Bootloader is loaded into 0x7C00
org	0x7C00
bits	16

; /n /r
%define ENDL 0x0D, 0x0A

; Yes it's a bit annoying to have to include the path but that's NASM
; Do let me know if there is a smarter way to do it!
%include "source_files/boot/FAT12_header.asm"

; Note - don't rename this label. It has to match the instruction in the header
start:
	; Initialize ds/es - depending on the BIOS they may not be set to 0
	mov	ax, 0
	mov	ds, ax
	mov	es, ax

	; Setup stack below our bootloader
	mov	ss, ax
	mov	sp, 0x7C00

	; Perform a far return to make sure that the segment is 0
	; And the offset is 0x7C00, rather than the other way around.
	push	es
	push	word	.continue
	retf

.continue:
	mov	[fat_drive_num], dl	; Save the drive number

	
	; Disk load message
	mov	si, load_msg
	call	prints

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

	; Find kernel.bin!
	xor	bx, bx ; 0
	mov	di, buffer

.find_kernel:
	mov	si, kernel_filename
	mov	cx, 11			; memcmp length
	push	di	; save di, as we will modify it and this will loop a lot

	; compares si and di, increments them
	; stops if cx reaches 0. also decreases cx with each iteration
	; Basically this is the x86 asm equivalent of doing this in C:
	;	while (si[i] == di[i] && cx-- != 0)
	;		++ i
	repe	cmpsb
	pop	di
	je	.found_kernel ; jump if equal

	add	di, 32	; 32 is the size of a directory entry
	inc	bx
	cmp	bx, [fat_dir_entry_count] ; Check that we are below the amount
	jl	.find_kernel

	.not_found_kernel:
		jmp	find_kernel_failed

.found_kernel:
	; di is still pointing to the dir entry
	mov	ax, [di + 26]	; 26th bit onward is the low 16b cluster entry
				; ie. the first cluster entry
	mov	[kernel_cluster], ax

	; load FAT
	mov	ax, [fat_reserved_sectors]
	mov	cl, [fat_sectors_per_fat]
	mov	bx, buffer
	call	read_disk

	mov	bx, KERNEL_SEGMENT
	mov	es, bx
	mov	bx, KERNEL_OFFSET

.load_kernel_loop:

	; first cluster = (kernel_cluster - 2) * sectors_per_cluster + start_sector
	mov	ax, [kernel_cluster]
	add	ax, 31			; TODO hardcoded fix later!!!!!!

	mov	cl, 1
	mov	dl, [fat_drive_num]
	call	read_disk

	add	bx, [fat_bytes_per_sector] ; TODO will overflow if kernel > 64kb

	; Get location of next cluster to read
	; cluster * 3 / 2
	mov	ax, [kernel_cluster]
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
	jae	.load_kernel_success

	; Not there yet :<
	mov	[kernel_cluster], ax
	jmp	.load_kernel_loop


.load_kernel_success:
	mov	dl, [fat_drive_num]
	mov	ax, KERNEL_SEGMENT
	mov	ds, ax ; Align segments to new location
	mov	es, ax
	jmp	KERNEL_SEGMENT:KERNEL_SEGMENT



; Yes it's a bit annoying to have to include the path but that's NASM
; Do let me know if there is a smarter way to do it!
%include "source_files/boot/util_read_disk.asm"

load_msg: db 'DOROS is loading...', ENDL, 0
find_kernel_fail_msg: db 'Kernel not found :<', ENDL, 0
kernel_filename: db 'KERNEL  BIN'
kernel_cluster: dw 0

KERNEL_SEGMENT	equ 0x2000
KERNEL_OFFSET	equ 0x0



; x86 recognizes where to boot by looking for 0x55aa between bits 510 to 512
; This adds 0s to memory until it reaches the 510th bit, then we define 0x55aa
times 510 - ($ - $$) db 0
db 0x55, 0xaa

buffer:

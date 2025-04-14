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

	; Load the kernel into memory
	mov	si, kernel_filename ; file to find
	call	load_file

	; Kernel is now in memory - JUMP!
	mov	dl, [fat_drive_num]
	mov	ax, KERNEL_SEGMENT
	mov	ds, ax ; Align segments to new location
	mov	es, ax
	jmp	KERNEL_SEGMENT:KERNEL_SEGMENT


; Yes it's a bit annoying to have to include the path but that's NASM
; Do let me know if there is a smarter way to do it!
%include "source_files/boot/util_read_disk.asm"
%include "source_files/boot/util_load_file.asm"

load_msg: db 'Loading DOROS', ENDL, 0
kernel_filename: db 'KERNEL  BIN'

KERNEL_SEGMENT	equ 0x2000
KERNEL_OFFSET	equ 0x0

; x86 recognizes where to boot by looking for 0x55aa between bits 510 to 512
; This adds 0s to memory until it reaches the 510th bit, then we define 0x55aa
times 510 - ($ - $$) db 0
db 0x55, 0xaa

buffer:

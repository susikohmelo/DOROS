; A memory offset is needed
[org 0x7c00]
[bits 16]

; Locations and size of important data like the GDT and kernel
GDT_LOCATION		equ 0x00000800
GDT_SECT_SIZE		equ 1

KERNEL_LOCATION		equ 0x7e00
KERNEL_SECT_SIZE	equ 42

section .text
global _start:

	mov	[disk_num], dl ; Save the disk number

	; Boot message
	mov	bx, boot_msg
	mov	[message_ptr], bx
	call	print_message

	; Wait for input
	mov	ah, 0x0
	int	0x16

	; Reading GDT to the correct location from the disk
	mov	ah, 0x02		; BIOS read sectors mode
	mov	al, GDT_SECT_SIZE	; N sectors to read
	mov	ch, 0x00 		; Cylinder num
	mov	dh, 0x00		; Head num
	mov	cl, 0x02		; Sector num
	mov	dl, [disk_num]		; Disk num
	mov	bx, GDT_LOCATION	; Location to read memory into
	int	0x13 			; Read disk
	; Read error checks
	jc	read_error ; Jump if carry
	cmp	al, GDT_SECT_SIZE ; Num of sectors read
	jne	read_error

	; Reading the kernel to the correct location from the disk
	mov	ah, 0x02		; BIOS read sectors mode
	mov	al, KERNEL_SECT_SIZE	; N sectors to read
	mov	ch, 0x00 		; Cylinder num
	mov	dh, 0x00		; Head num
	mov	cl, 0x02 + GDT_SECT_SIZE ; Sector num
	mov	dl, [disk_num]		; Disk num
	mov	bx, KERNEL_LOCATION	; Location to read memory into
	int	0x13 			; Read disk
	; Read error checks
	jc	read_error ; Jump if carry
	cmp	al, KERNEL_SECT_SIZE ; Num of sectors read
	je	read_success

	read_error:
		mov	bx, read_error_msg
		mov	[message_ptr], bx
		call	print_message
		jmp	$

	read_success:
		; Read succesful message
		mov	bx, read_success_msg
		mov	[message_ptr], bx
		call	print_message

	; Wait for input
	mov	ah, 0x0
	int	0x16

	; Enter text mode - this clears the screen for us
	mov	ah, 0x0
	mov	al, 0x3
	int	0x10

	; ENTER PROTECTED MODE ! -------------
	cli	; Clear interrupts
	lgdt	[GDTD] ; Load GDT

	; Change the last bit of the cr0 register to 1
	mov		eax, cr0 
	or		eax, 1
	mov		cr0, eax ; Now we are in 32bit mode

	mov		eax, KERNEL_LOCATION
	jmp		eax


[bits 16]
print_message:
	mov	ah, 0x0e
	mov	bx, [message_ptr]
	print_loop:
		mov	al, [bx]
		int	0x10
		inc	bx
		cmp	al, 0x0
		jne	print_loop
		ret		

message_ptr: ; Temp memory to store print parameter in
	times 2 db 0

boot_msg:
	db 0xa, 0xd ; \n \r
	db "DOROS bootloader found!"
	db 0xa, 0xa, 0xd ; \n \n \r
	db "Press any key to read the disk & load DOROX kernel into memory."
	db 0xa, 0xd ; \n \r
	db "Waiting for input ..."
	db 0xa, 0xd, 0 ; \n \r NULL

read_error_msg:
	db "Error reading disk :<", 0

read_success_msg:
	db 0xa, 0xd ; \n \r
	db "Success!"
	db 0xa, 0xa, 0xd ; \n \n \r
	db "Press any key to enter 32-bit protected mode & boot into DOROS."
	db 0xa, 0xd ; \n \r
	db "Waiting for input ...", 0

disk_num: ; Variable for storing the disk the boot sector is located in
	db 0

GDTD:
	dw	GDT_end - GDT_start - 1 ; Size of the GDT
	dd	GDT_LOCATION

; x86 recognizes where to boot by looking for 55 aa between bits 510 to 512
; This adds 0s to memory until it reaches the 510th bit, then we define 55/aa
times 510 - ($ - $$) db 0
db 0x55, 0xaa

GDT_FILE_START:
%include"source_files/boot/GDT.asm"
GDT_FILE_END:
; This is 0 padding to make the GDT 512 bits long.
; It makes it easy to add/remove things from the GDT as well as load it into a
; specific memory address by reading the sector via bios
times 512 - (GDT_FILE_END - GDT_FILE_START) db 0x0

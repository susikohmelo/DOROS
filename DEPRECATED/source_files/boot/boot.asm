; A memory offset is needed
[org 0x7c00]
[bits 16]

; Location the kernel is loaded into
KERNEL_LOCATION equ 0x7e00
KERNEL_SECTOR_SIZE equ 42

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

	; Reading sector 2 from the disk
	mov	ah, 0x02		; BIOS read sectors mode
	mov	al, KERNEL_SECTOR_SIZE	; N sectors to read
	mov	ch, 0x00 		; Cylinder num
	mov	dh, 0x00		; Head num
	mov	cl, 0x02		; Sector num
	mov	dl, [disk_num]		; Disk num
	mov	bx, KERNEL_LOCATION	; Location to read memory into
	int	0x13 			; Read disk

	; Read error checks
	jc	read_error ; Jump if carry
	cmp	al, KERNEL_SECTOR_SIZE ; Num of sectors read
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

	jmp	setup_protected_mode


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

%include"source_files/boot/enter_protected_mode.asm"

; x86 recognizes where to boot by looking for 55 aa between bits 510 to 512
; This adds 0s to memory until it reaches the 510th bit, then we define 55/aa
times 510 - ($ - $$) db 0
db 0x55, 0xaa

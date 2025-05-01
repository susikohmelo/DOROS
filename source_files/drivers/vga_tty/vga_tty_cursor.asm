global enable_cursor
global disable_cursor
global move_cursor

bits 32

enable_cursor:
	pushf
	push	eax
	push	edx

	; A funnny property of the VGA registers.
	; To access them, you set a value into a port (such as 0x3D4)
	; and then into the next field the actual stuff you want to read/write
	mov	dx, 0x3D4
	mov	al, 0xA ; cursor shape register
	out	dx, al

	inc	dx ; 0x3D5
	; Bit structure of the register:
	; [ unused 3 bits ] [dis/enable cursor 1 bit] [cursor shape 4 bits]
	mov	al, 0x00 ; Controls height of the cursor. 0x00 to 0xF
	out	dx, al

	pop	edx
	pop	eax
	popf
	ret

disable_cursor:
	pushf
	push	eax
	push	edx

	; A funnny property of the VGA registers.
	; To access them, you set a value into a port (such as 0x3D4)
	; and then into the next field the actual stuff you want to read/write
	mov	dx, 0x3D4
	mov	al, 0xA ; cursor shape register
	out	dx, al

	inc	dx ; 0x3D5
	; Bit structure of the register:
	; [ unused 3 bits ] [dis/enable cursor 1 bit] [cursor shape 4 bits]
	mov	al, 0x20
	out	dx, al

	pop	edx
	pop	eax
	popf
	ret

VGA_WIDTH equ 80
; Takes 2 params, X and Y. 8 bits each.
move_cursor:
	mov	bx, [esp + 4]	; X
	mov	ax, [esp + 8]	; y

	mov	dl, VGA_WIDTH
	mul	dl		; (y * width)
	add	bx, ax		; (y + x).

	; Actually setting the values in the IO memory
	; The bits are split in two areas. It's dumb but what can you do.
	; 0x0F has the first half, 0x0E is the second.
	mov	dx, 0x3D4
	mov	al, 0x0F
	out	dx, al
	
	inc	dl ; 0x3D5
	mov	al, bl ; position lowbits
	out	dx, al

	dec	dl ; 0x3D4
	mov	al, 0x0E
	out	dx, al

	inc	dl
	mov	al, bh ; position highbits
	out	dx, al
	ret

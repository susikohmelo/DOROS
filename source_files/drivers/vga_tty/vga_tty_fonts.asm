global get_fonts
global set_fonts

bits 32

; in: ptr to buffer with 4096 bytes
get_fonts:
	mov	edi, [esp + 4]
	; clear even/odd mode
	mov		dx, 0x3CE
	mov		ax, 5
	out		dx, ax

	; map VGA memory to 0xa0000
	mov		ax, 0x406
	out		dx, ax

	; set bitplane 2
	mov		dx, 0x3C4
	mov		ax, 0x402
	out		dx, ax

	; clear even/odd mode (the other way)
	mov		ax, 0x604
	out		dx, ax

	; copy charmap
	mov		esi, 0xa0000
	mov		ecx, 128

load_loop:
	; copy 16 bytes to bitmap
	movsd
	movsd
	movsd
	movsd
	; skip 16 bytes
	add		esi, 16
	loop		load_loop

	; restore VGA state to normal operation
	mov		ax, 0x302
	out		dx, ax
	mov		ax, 0x204
	out		dx, ax
	mov		dx, 0x3CE
	mov		ax, 0x1005
	out		dx, ax
	mov		ax, 0xE06
	out		dx, ax
	ret


; in: ptr to buffer with 4096 bytes
set_fonts:
	mov	esi, [esp + 4]
	; clear even/odd mode
	mov		dx, 0x3CE
	mov		ax, 5
	out		dx, ax

	; map VGA memory to 0xa0000
	mov		ax, 0x406
	out		dx, ax

	; set bitplane 2
	mov		dx, 0x3C4
	mov		ax, 0x402
	out		dx, ax

	; clear even/odd mode (the other way)
	mov		ax, 0x604
	out		dx, ax

	; copy charmap
	mov		edi, 0xa0000
	mov		ecx, 128

load_loop2:
	; copy 16 bytes to bitmap
	movsd
	movsd
	movsd
	movsd
	; skip 16 bytes
	add		esi, 16
	loop		load_loop2

	; restore VGA state to normal operation
	mov		ax, 0x302
	out		dx, ax
	mov		ax, 0x204
	out		dx, ax
	mov		dx, 0x3CE
	mov		ax, 0x1005
	out		dx, ax
	mov		ax, 0xE06
	out		dx, ax
	ret

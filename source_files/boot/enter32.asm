bits	16
org	0x0
; ^ DS is our offset now, no need for this

	; Enter text mode - this clears the screen for us
	mov	ah, 0x0
	mov	al, 0x3
	int	0x10

	; ENTER PROTECTED MODE ! -------------
	cli	; Clear interrupts
	lgdt	[GDT_descriptor] ; Load GDT

	; Change the last bit of the cr0 register to 1
	mov		eax, cr0 
	or		eax, 1
	mov		cr0, eax ; Now we are in 32bit mode

	jmp	dword CODE_SEG:0x2000*16+after_protected_mode

; Some padding so that we know GDT starts at exactly kernel_start + 64
; times 64 - ($ - $$) db 0

[bits 16]
%include"source_files/boot/GDT.asm"

[bits 32]
after_protected_mode:
	mov	al, 'A'
	mov	ah, '0x0f'
	mov	[0xb8000], ax
	jmp	$

; ================================================
; HERE WILL BE APPENDED AN ASM FILE IN THE MAKEFILE

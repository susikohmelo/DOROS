;
; This file is responsible for loading the GDT and entering 32 bit mode.
; This is loaded at KERNEL_LOCATION which is hardcoded as 0x20000.
;
; A very small amount of memory is wasted by having this function loaded in,
; but it made entering protected mode far simpler.
;
; Something to improve upon later for sure! But I think for now I can live with
; losing maybe 20 bytes out of the 40,000,000.


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

	jmp	dword CODE_SEG:0x20000+after_protected_mode


%include"source_files/boot/GDT.asm"

bits 32
after_protected_mode:

	mov	ax, DATA_SEG
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	
	; Setting up the stack pointers
	mov	ebp, 0x90000 ; TODO arbitrary number, look more into later
	mov	esp, ebp


; This padding is here so that we know exactly where the real kernel starts
; (0x20000 + 512 = 0x20200)
; Yes - this is kind of stupid - but it can be easily changed later.
times 512 - ($ - $$) db 0

; ================================================
; HERE WILL BE APPENDED THE KERNELS MAIN IN THE MAKEFILE
; 
; The real kernel will begin here

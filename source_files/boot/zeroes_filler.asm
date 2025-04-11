; This file just fills memory with 0s. It's needed for virtual machines

; In the bootloader, we do not know the exact size of the kernel.
; So we may read a little past the kernel. On hardware this is fine, but we -
; run into problems on many virtual machines. Think of this like padding.

; This memory is not wasted - it will get written over by the kernel later 

times 42000 db 0

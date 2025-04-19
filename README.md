# DOROS
A simple x86 32-bit OS written from scratch in Assembly and C.

<sub> DOROS booting on real hardware <sub/><br>
<img src="https://github.com/susikohmelo/DOROS/blob/main/readme_files/doros_boot.gif" height="400" />


Status: Highly WIP - expect to see a lot more stuff in a month or two!

## What is currently implemented?
- Bootloader with an assembly based driver to find and load any file from a FAT12 file system
- Global Descriptor Table (GDT) following a small flat memory model
- Entry to 32-bit protected mode and the C based kernel
- Basic TTY functionality (printing characters to video memory with colors)
- Interrupt Descriptor Table (IDT)
- Interrupt based keyboard driver (though it is printing the raw keycodes currently and still needs mapping)

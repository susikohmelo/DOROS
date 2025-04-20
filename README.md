# DOROS
A simple x86 32-bit OS written from scratch in Assembly and C.

<sub> DOROS booting on real hardware <sub/><br>
<img src="https://github.com/susikohmelo/DOROS/blob/main/readme_files/doros_boot.gif" height="400" />


Status: Highly WIP - expect to see a lot more stuff in a month or two!

## What's currently implemented?
### In 16-bit real mode
- Bootloader with an assembly based driver to find and load any file from a FAT12 file system
- Global Descriptor Table following a small flat memory model
### In 32-bit protected mode
- Interrupt Descriptor Table
- Basic TTY functionality (printing characters to video memory with colors)
- Interrupt based PS/2 keyboard driver
- Heap allocation (malloc/free like functionality)
- **Currently working on: an extremely basic shell**

## How to run?
Compiling the program is a bit harder when it comes to OS's, since the system you're compiling it on likely has a different arcitechture from the target.

Currently the repository doesn't contain a script or docker image that would allow you to install the tools required to compile the code.

You would have to install the gcc i386 cross compiler yourself.
This will be added in the near future, or at the very least a precompiled binary.

The output of the makefile is a file called floppy_DOROS.img.

It is a FAT12 floppy disk image. You can simply run it in QEMU as is, or if you want to try it on real hardware,
rename the extension to .iso and burn it onto a USB stick to boot off of. Yes floppy images convert directly to ISO images.

# DOROS
A simple x86 32-bit OS written from 100% scratch in Assembly and C.
>[!WARNING]
This project is WIP - expect to see more later, that being said there's already plenty to look at!

<sub> DOROS booting on real hardware </sub><br>
<img src="https://github.com/susikohmelo/DOROS/blob/main/readme_files/doros_boot.gif" height="400" />

>[!TIP]
**The repository is well documented** - feel free to look around!<br><br>
Most of the source_file subfolders have READMEs with a summary of the folder's contents.<br>


## What's currently implemented?
### In 16-bit real mode
- Bootloader with an assembly based driver to find and load any file from a FAT12 file system
- Global Descriptor Table following a small flat memory model
### In 32-bit protected mode
- Interrupt Descriptor Table
- Basic TTY functionality (printing characters to video memory with colors)
- Interrupt based PS/2 keyboard driver
- Heap allocation (malloc/free like functionality)
- Basic shell
- **Currently working on:** VGA cursor controls

## How to run?
Compiling the program is a bit harder when it comes to OS's, since the system you're compiling it on likely has a different arcitechture from the target.

Currently the repository doesn't contain a script or docker image that would allow you to install the tools required to compile the code.

You would have to install the gcc i386 cross compiler yourself. A script for this will be added in the near future.

**However, there is a precompiled disk image included in the repository!**

It is a FAT12 floppy disk image. You can install QEMU and run it with `make run` or if you want to try it on real hardware,
rename the extension to .iso and burn it onto a USB stick to boot off of. Indeed, floppy images convert directly to ISO images.

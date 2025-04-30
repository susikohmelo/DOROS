# DOROS
A simple x86 32-bit OS written from 100% scratch in Assembly and C.
>[!NOTE]
This project is WIP - expect to see more with time. That being said there's already plenty to look at!

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
- Interrupt based PS/2 keyboard driver
- Basic TTY functionality (printing characters to video memory with colors)
- Heap allocation (malloc/free like functionality)
- Basic shell & basic commands for it
- **Currently working on:** VGA cursor controls

## How to compile?

### Option 1: Use the precompiled image instead.
There is a FAT12 floppy image called `floppy_DOROS.img` inside the `precompiled_OS` folder. You can skip the compiling and just use that.

### Option 2: Using Docker
Running `make docker` will build & run a docker image, which will compile the code for you.<br>
Why docker? Compiling systems is pretty hard as you're building code for a different computer. It requires special tools like cross compilers which are a bit of a pain to install and will likely be never used for anything else. Docker is an elegant solution to this, but be warned - it takes a WHILE to build the environment!

## How to run?
Install `QEMU` and run the OS with `make run`.<br>
Alternatively you can try it on real hardware! Rename the extension of the `precompiled_OS/floppy_DOROS.img` to .iso and burn it onto a USB stick to boot off of. Indeed, floppy images convert directly to ISO images.


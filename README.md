# DOROS
A simple x86 32-bit OS written from scratch in Assembly and C.
>[!NOTE]
This project is WIP - expect to see more with time. That being said there's already plenty to look at!

<sub> DOROS booting on real hardware </sub><br>
<img src="https://github.com/susikohmelo/DOROS/blob/main/readme_files/doros_boot.gif"/>

>[!TIP]
**The repository is well documented** - feel free to look around!<br><br>
Most of the source_file subfolders have READMEs with a summary of the folder's contents.<br>


## What's currently implemented?
### In 16-bit real mode
- Bootloader with an assembly based driver to find and load any file from a FAT12 file system
- GDT following a small flat memory model (ie. up to 4gb ram)
### In 32-bit protected mode
- IDT & everything needed for interrupts (such as PIC configs & IVT)
- PS/2 keyboard driver
- PS/2 mouse driver
- TUI mouse cursor (font bitmask magic to give an illusion of a real GUI mouse)
- Heap allocation (malloc/free like functionality)
- Basic TTY functionality (VGA text mode terminal)
- Basic shell & commands

## How to compile?

### Option 1: Use the precompiled image instead.
There is a FAT12 floppy image called `floppy_DOROS.img` inside the `precompiled_OS` folder. You can skip the compiling and just use that.

### Option 2: Using Docker
Running `make docker` will build & run a docker image, which will compile the code for you.<br>
Why docker? Compiling systems is pretty hard as you're building code for a different computer. It requires special tools like cross compilers which are a bit of a pain to install and will likely be never used for anything else. Docker is an elegant solution to this, but be warned - it takes a WHILE to build the environment!

## How to run?
Install `QEMU` and run the OS with `make run`.<br>
Alternatively you can try it on real hardware! Burn the `precompiled_OS/floppy_DOROS.img` onto a USB stick and
select it in your boot menu. Make sure that BIOS is enabled and not UEFI!


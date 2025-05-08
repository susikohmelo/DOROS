# DOROS
A simple x86 32-bit OS written from scratch in Assembly and C.

>[!TIP]
**The repository is well documented** - feel free to look around!<br><br>
Most of the source_file subfolders have READMEs with a summary of the folder's contents.<br>

<sub> DOROS booting & running on real hardware.</sub><br>
<img src="https://github.com/susikohmelo/DOROS/blob/main/readme_files/doros_boot.gif" height="350"/>
<img src="https://github.com/susikohmelo/DOROS/blob/main/readme_files/doros_mouse.gif" height="350"/>

<sub> Example of basic artwork that can be made inside the paint program.</sub><br>
<img src="https://github.com/susikohmelo/DOROS/blob/main/readme_files/image.webp" height="200"/>


## What's currently implemented?
### In 16-bit real mode
- FAT12 file system (only accessible in 16bit mode)
- Bootloader with an assembly based driver to find and load any file from the file system
- GDT following a small flat memory model (ie. up to 4gb of unsegmented kernel space ram)

### In 32-bit protected mode
- IDT & everything needed for interrupts (such as PIC configs)
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
For linux/unix, install `qemu-system` and run the OS with `make run`.<br>
For windows, install qemu via other means and run the emulator with `precompiled_OS/floppy_DOROS.img` as the input argument.

Alternatively you can try it on real hardware! Burn the `precompiled_OS/floppy_DOROS.img` onto a USB stick and
select it in your boot menu. Make sure that BIOS is enabled instead of UEFI!


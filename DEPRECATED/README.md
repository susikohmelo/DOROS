# OLD IMPLEMENTATION OF BOOTLOADER

This is a very simplistic, purely assembly based bootloader that does the job.
It loads the kernel, sets the cpu to 32 bit mode and jumps into a .c program.

Instead of removing it I'm keeping it as there is a lot of useful documentation
in the code, especially in the GDT file.

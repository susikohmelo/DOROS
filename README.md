# DOROS
A simple x86 32-bit OS written from scratch in Assembly and C.

The main components are the **Kernel**, **Bootloader** and **Shell**.

Status: Highly WIP - expect to see a lot more stuff in a month or two!

## What is currently implemented?
- Bootloader with an assembly based driver to find and load any file from a FAT12 file system
- Small flat memory model GDT loaded into memory and the CPU
- Entry to 32-bit protected mode
- (CURRENTLY WORKING ON) Basic TTY functionality for the C based kernel

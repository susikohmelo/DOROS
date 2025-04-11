
BOOT_SRC = source_files/boot/
KERN_SRC = source_files/kernel/
BIN_LOC  = binaries/

# C cross compiler
CC = i386-elf-gcc -ffreestanding -m32 -g
# Linker used
LD = i386-elf-ld

all:
	# Compile bootloader into a bin
	nasm -f bin $(BOOT_SRC)boot.asm -o $(BIN_LOC)DOROS.bin
	#
	# Compile ASM kernel entrypoint
	nasm -f elf $(BOOT_SRC)enter_kernel.asm -o $(BIN_LOC)enter_kernel.o
	#
	# Compile C kernel
	$(CC) -c $(KERN_SRC)kernel.c -o $(BIN_LOC)kernel.o
	#
	# This just pads memory with 0s. It's needed for virtual machines
	nasm -f bin $(BOOT_SRC)zeroes_filler.asm -o $(BIN_LOC)zeroes_filler.bin
	#
	# Link ASM entrypoint and C kernel together
	$(LD) -o $(BIN_LOC)kernel.bin -Ttext 0x7E00 $(BIN_LOC)enter_kernel.o \
		$(BIN_LOC)kernel.o --oformat binary
	#
	# Append kernel binary and the 0 padding to the main binary
	cat $(BIN_LOC)kernel.bin $(BIN_LOC)zeroes_filler.bin >> $(BIN_LOC)DOROS.bin

run : all
	qemu-system-x86_64 -drive format=raw,file=$(BIN_LOC)"DOROS.bin",index=0,if=floppy,  -m 128M

clean:
	rm $(BIN_LOC)*.bin
	rm $(BIN_LOC)*.o

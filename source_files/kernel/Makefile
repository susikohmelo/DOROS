BUILD_DIR  = build/

# Assembler
ASM = nasm

# C cross compiler
CC = i386-elf-gcc -ffreestanding -m32 -g -O2 -std=gnu99  -ggdb
# Linker used
# LD = i386-elf-ld

.PHONY: all clean always

all: always
	$(CC) -c kernel.c -o $(BUILD_DIR)kernel_c.o
	$(CC) -c heap/kmalloc.c -o $(BUILD_DIR)kmalloc.o
	$(CC) -c heap/kfree.c -o $(BUILD_DIR)kfree.o
	$(CC) -c boot_message.c -o $(BUILD_DIR)boot_message.o
	$(CC) -c interrupts/IDT.c -o $(BUILD_DIR)IDT.o
	$(ASM) -f elf interrupts/interrupt_utils.asm -o $(BUILD_DIR)interrupt_utils.o

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

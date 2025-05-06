BUILD_DIR  = build/

# Assembler
ASM = nasm

INCLUDES= -I. -I../drivers/include -I../kernel -I../kernel/interrupts -I../kernel/heap

# C cross compiler
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc -ffreestanding -m32 -g -O2 -std=gnu99 -ggdb $(INCLUDES)
# Linker used
# LD = i386-elf-ld

.PHONY: all clean always

all: always
	$(CC) -c picoshell.c -o $(BUILD_DIR)picoshell.o

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

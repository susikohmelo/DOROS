BUILD_DIR  = build/

# Assembler
ASM = nasm

# C cross compiler
CC = i386-elf-gcc -ffreestanding -m32 -g -O2 -std=gnu99  -ggdb
# Linker used
# LD = i386-elf-ld

.PHONY: all clean always

all: always
	$(CC) -c vga_tty/vga_tty_printing.c -o $(BUILD_DIR)tty.o
	$(CC) -c keyboard/keyboard.c -o $(BUILD_DIR)keyboard.o
	$(ASM) -f elf keyboard/receive_keyboard_interrupts.asm -o $(BUILD_DIR)receive_keyboard_interrupts.o

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

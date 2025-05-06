BUILD_DIR  = build/

# Assembler
ASM = nasm

INCLUDES= -I./include -I../shell -I../kernel/interrupts -I../kernel/heap

# C cross compiler
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc -ffreestanding -m32 -g -O2 -std=gnu99 -ggdb $(INCLUDES)
# Linker used
# LD = i386-elf-ld

.PHONY: all clean always

all: always
	$(CC) -c vga_tty/vga_tty_printing.c -o $(BUILD_DIR)tty.o
	$(CC) -c keyboard/keyboard.c -o $(BUILD_DIR)keyboard.o
	$(CC) -c mouse/mouse.c -o $(BUILD_DIR)mouse.o
	$(ASM) -f elf keyboard/receive_keyboard_interrupts.asm -o $(BUILD_DIR)receive_keyboard_interrupts.o
	$(ASM) -f elf mouse/receive_mouse_interrupts.asm -o $(BUILD_DIR)receive_mouse_interrupts.o
	$(ASM) -f elf vga_tty/vga_tty_cursor.asm -o $(BUILD_DIR)vga_tty_cursor.o

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

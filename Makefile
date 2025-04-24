
BOOT_SRC = source_files/boot/
KERN_SRC = source_files/kernel/
SHEL_SRC = source_files/shell/
BUILD_DIR  = build/
BLUEPRINT_DIR = blueprints/

# Assembler
ASM = nasm

# C cross compiler
CC = i386-elf-gcc -ffreestanding -m32 -g -O2 -std=gnu99  -ggdb
# Linker used
LD = i386-elf-ld

.PHONY: all floppy_img kernel bootloader clean always blueprints_fat

all: floppy_img blueprints_fat

# FLOPPY IMAGE ----------------------------------------------------------------
floppy_img: $(BUILD_DIR)floppy_DOROS.img

$(BUILD_DIR)floppy_DOROS.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)floppy_DOROS.img bs=512 count=2880
	mkfs.fat -F 12 -n "DOROS" $(BUILD_DIR)floppy_DOROS.img
	cat $(BUILD_DIR)enter32.bin $(BUILD_DIR)partial_kernel.bin > $(BUILD_DIR)kernel.bin
	dd if=$(BUILD_DIR)bootloader.bin of=$(BUILD_DIR)floppy_DOROS.img conv=notrunc
	mcopy -i $(BUILD_DIR)floppy_DOROS.img $(BUILD_DIR)kernel.bin "::kernel.bin"


# BOOTLOADER ------------------------------------------------------------------
bootloader: $(BUILD_DIR)bootloader.bin

$(BUILD_DIR)bootloader.bin: always
	$(ASM) -f bin $(BOOT_SRC)bootmaster.asm -o $(BUILD_DIR)bootloader.bin
	$(ASM) -f bin $(BOOT_SRC)enter32.asm -o $(BUILD_DIR)enter32.bin


# KERNEL ----------------------------------------------------------------------
kernel: $(BUILD_DIR)kernel.bin

# TODO this is kind of messy and should be cut into smaller makefiles
$(BUILD_DIR)kernel.bin: always
	$(CC) -c $(KERN_SRC)kernel.c -o $(BUILD_DIR)kernel_c.o
	$(CC) -c $(KERN_SRC)heap/kmalloc.c -o $(BUILD_DIR)kmalloc.o
	$(CC) -c $(KERN_SRC)heap/kfree.c -o $(BUILD_DIR)kfree.o
	$(CC) -c $(KERN_SRC)boot_message.c -o $(BUILD_DIR)boot_message.o
	$(CC) -c $(SHEL_SRC)picoshell.c -o $(BUILD_DIR)picoshell.o
	# NOTE! This is highly temporary and just for testing
	# This needs it's own makefile later
	$(CC) -c source_files/drivers/vga_tty/vga_tty_printing.c -o $(BUILD_DIR)tty.o
	$(CC) -c source_files/drivers/keyboard/keyboard.c -o $(BUILD_DIR)keyboard.o
	$(CC) -c source_files/kernel/interrupts/IDT.c -o $(BUILD_DIR)IDT.o
	$(ASM) -f elf source_files/kernel/interrupts/interrupt_utils.asm -o $(BUILD_DIR)interrupt_utils.o
	$(ASM) -f elf source_files/drivers/keyboard/receive_keyboard_interrupts.asm -o $(BUILD_DIR)receive_keyboard_interrupts.o
	# Yes 0x20200 is hardcoded. It is the location of the kernel
	$(LD) -o $(BUILD_DIR)partial_kernel.bin -Ttext 0x20200 \
		$(BUILD_DIR)kernel_c.o $(BUILD_DIR)boot_message.o\
		$(BUILD_DIR)keyboard.o $(BUILD_DIR)receive_keyboard_interrupts.o \
		$(BUILD_DIR)interrupt_utils.o $(BUILD_DIR)IDT.o \
		$(BUILD_DIR)kmalloc.o $(BUILD_DIR)kfree.o \
		$(BUILD_DIR)picoshell.o \
		$(BUILD_DIR)tty.o --oformat binary


# MISC ------------------------------------------------------------------------
always:
	mkdir -p $(BUILD_DIR)

run:
	qemu-system-i386 -fda $(BUILD_DIR)floppy_DOROS.img

debug:
	bochs -f bochs_config

clean:
	rm -rf $(BUILD_DIR)*

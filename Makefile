
BOOT_SRC = source_files/boot/
KERN_SRC = source_files/kernel/
BUILD_DIR  = build/
BLUEPRINT_DIR = blueprints/

# Assembler
ASM = nasm

# C cross compiler
CC = i386-elf-gcc -ffreestanding -m32 -g -O2 -std=gnu99 -Werror
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

$(BUILD_DIR)kernel.bin: always
	$(CC) -c $(KERN_SRC)kernel.c -o $(BUILD_DIR)kernel.o
	# NOTE! This is highly temporary and just for testing
	$(CC) -c source_files/libk/vga_tty/vga_tty_printing.c -o $(BUILD_DIR)lib.o
	# Yes 0x20200 is hardcoded. It is the location of the kernel
	$(LD) -o $(BUILD_DIR)partial_kernel.bin -Ttext 0x20200 \
		$(BUILD_DIR)kernel.o $(BUILD_DIR)lib.o --oformat binary


# MISC ------------------------------------------------------------------------
always:
	mkdir -p $(BUILD_DIR)

run:
	qemu-system-i386 -fda $(BUILD_DIR)floppy_DOROS.img

debug:
	bochs -f bochs_config

clean:
	rm -rf $(BUILD_DIR)*


BOOT_SRC = source_files/boot/
KERN_SRC = source_files/kernel/
BUILD_DIR  = build/
BLUEPRINT_DIR = blueprints/

# Assembler
ASM = nasm

# C cross compiler
CC = i386-elf-gcc -ffreestanding -m32 -g
# Linker used
LD = i386-elf-ld

.PHONY: all floppy_img kernel bootloader clean always blueprints_fat

all: floppy_img blueprints_fat

# FLOPPY IMAGE ----------------------------------------------------------------
floppy_img: $(BUILD_DIR)floppy_DOROS.img

$(BUILD_DIR)floppy_DOROS.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)floppy_DOROS.img bs=512 count=2880
	mkfs.fat -F 12 -n "DOROS" $(BUILD_DIR)floppy_DOROS.img
	dd if=$(BUILD_DIR)bootloader.bin of=$(BUILD_DIR)floppy_DOROS.img conv=notrunc
	mcopy -i $(BUILD_DIR)floppy_DOROS.img $(BUILD_DIR)kernel.bin "::kernel.bin"
	mcopy -i $(BUILD_DIR)floppy_DOROS.img test.txt "::test.txt"


# BOOTLOADER ------------------------------------------------------------------
bootloader: $(BUILD_DIR)bootloader.bin

$(BUILD_DIR)bootloader.bin: always
	$(ASM) -f bin $(BOOT_SRC)bootmaster.asm -o $(BUILD_DIR)bootloader.bin


# KERNEL ----------------------------------------------------------------------
kernel: $(BUILD_DIR)kernel.bin

$(BUILD_DIR)kernel.bin: always
	$(ASM) -f bin $(KERN_SRC)kernel.asm -o $(BUILD_DIR)kernel.bin


# MISC ------------------------------------------------------------------------
always:
	mkdir -p $(BUILD_DIR)

run:
	qemu-system-i386 -fda $(BUILD_DIR)floppy_DOROS.img

debug:
	bochs -f bochs_config

blueprints_fat: $(BLUEPRINT_DIR)FAT12/FAT12.c always
	mkdir -p $(BUILD_DIR)$(BLUEPRINT_DIR)
	gcc -g $(BLUEPRINT_DIR)FAT12/FAT12.c -o $(BUILD_DIR)$(BLUEPRINT_DIR)fat


clean:
	rm -rf $(BUILD_DIR)*


BOOT_SRC = source_files/boot/
KERN_SRC = source_files/kernel/
SHEL_SRC = source_files/shell/
DRIV_SRC = source_files/drivers/
BUILD_DIR  = build/
FLOPPY_DIR  = precompiled_os/
BLUEPRINT_DIR = blueprints/

# Assembler
ASM = nasm

# C cross compiler
CC = i386-elf-gcc -ffreestanding -m32 -g -O2 -std=gnu99  -ggdb
# Linker used
LD = i386-elf-ld

.PHONY: all floppy_img kernel bootloader clean always blueprints_fat docker

all: floppy_img blueprints_fat

# FLOPPY IMAGE ----------------------------------------------------------------
floppy_img: $(BUILD_DIR)floppy_DOROS.img

$(BUILD_DIR)floppy_DOROS.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)floppy_DOROS.img bs=512 count=2880
	mkfs.fat -F 12 -n "DOROS" $(BUILD_DIR)floppy_DOROS.img
	cat $(BUILD_DIR)enter32.bin $(BUILD_DIR)partial_kernel.bin > $(BUILD_DIR)kernel.bin
	dd if=$(BUILD_DIR)bootloader.bin of=$(BUILD_DIR)floppy_DOROS.img conv=notrunc
	mcopy -i $(BUILD_DIR)floppy_DOROS.img $(BUILD_DIR)kernel.bin "::kernel.bin"
	mv $(BUILD_DIR)floppy_DOROS.img $(FLOPPY_DIR)


# BOOTLOADER ------------------------------------------------------------------
bootloader: $(BUILD_DIR)bootloader.bin

$(BUILD_DIR)bootloader.bin: always
	$(ASM) -f bin $(BOOT_SRC)bootmaster.asm -o $(BUILD_DIR)bootloader.bin
	$(ASM) -f bin $(BOOT_SRC)enter32.asm -o $(BUILD_DIR)enter32.bin


# KERNEL ----------------------------------------------------------------------
kernel: $(BUILD_DIR)kernel.bin

$(BUILD_DIR)kernel.bin: always
	cd $(KERN_SRC) && make
	cd $(DRIV_SRC) && make
	cd $(SHEL_SRC) && make
	# Yes 0x20200 is hardcoded. It is the location of the kernel
	$(LD) -o $(BUILD_DIR)partial_kernel.bin -Ttext 0x20200 \
		$(KERN_SRC)$(BUILD_DIR)*.o \
		$(DRIV_SRC)$(BUILD_DIR)*.o \
		$(SHEL_SRC)$(BUILD_DIR)*.o \
		--oformat binary

# MISC ------------------------------------------------------------------------
always:
	mkdir -p $(BUILD_DIR)

docker:
	cd docker && sudo docker compose up --build

run:
	# qemu-system-i386 -fda $(FLOPPY_DIR)floppy_DOROS.img
	qemu-system-x86_64 -fda $(FLOPPY_DIR)floppy_DOROS.img

debug:
	bochs -f bochs_config

clean:
	rm -rf $(BUILD_DIR)
	cd $(DRIV_SRC) && make clean
	cd $(SHEL_SRC) && make clean
	cd $(KERN_SRC) && make clean

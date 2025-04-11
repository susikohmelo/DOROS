
BOOT_SRC = source_files/boot/

all:
	nasm -f bin $(BOOT_SRC)boot.asm -o boot.bin

run : all
	qemu-system-x86_64 boot.bin

clean:
	rm boot.bin

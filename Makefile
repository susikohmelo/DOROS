

SOURCES = source_files/

all:
	nasm -f bin $(SOURCES)boot.asm -o boot.bin

run : all
	qemu-system-x86_64 boot.bin

clean:
	rm boot.bin

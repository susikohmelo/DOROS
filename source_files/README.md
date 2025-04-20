# How the OS launches
```
- BOOT -
1. Bootmaster finds & loads the kernel from the FAT12 filesystem
2. GDT is loaded into memory.
3. Entry to 32-bit mode

- KERNEL -
4. Kernel.c main() starts
5. IDT is loaded into memory
6. Drivers are hooked up to go

7. Terminal is initialized & boot message printed
```

## Points of interest

```
boot/
- Bootmaster
- FAT12 file loading
- GDT

kernel/
- kernel's main()
- heap allocation
- IDT

drivers/
- keyboard driver
- terminal driver
```

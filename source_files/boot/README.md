# Summary of files & their purpose
```
- bootmaster.asm -
The main boot file. Contains higher level logic for the boot.

- GDT.asm -
Contains the GDT itself.

- FAT12_header.asm -
A header used by the FAT12 filesystem.

- enter32.asm -
This loads the GDT into memory and makes the jump to 32-bit mode.

- util_*.asm
These files have general utility functions corresponding to their names.
FAT12 file finding, loading, disk reading and BIOS character printing respectively.
```

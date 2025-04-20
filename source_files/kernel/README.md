# Structure
```
- kernel.c -
Contains the primary abstract logic.

As much as possible the functions are contained within their own files and 
we are just calling them in order here in the kernel.c main().


- Interrupts/ -
Has the IDT and all the basic interrupt related functions.

- Heap/ -
Has everything related to heap memory management
such as kmalloc() and kfree()

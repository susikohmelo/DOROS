# Kernel's std C library

Though since the whole OS takes place in kernel space - it is 
essentially just the whole system's standard library.

Note - heap allocation such as kmalloc is found under ../kernel/heap/

Also note that this does not have drivers.
See ../drivers folder for things like mouse/key inputs etc.

# Heap allocation documentation ( kmalloc / kfree )

Very simplistic implementation of malloc like heap allocation.<br>
Instead of using arenas, this has one bitmap that represents the entire heap.

The bitmap itself is located right before the heap.

Arenas are better especially when paging is in play, but this is
something that can be easily changed later internally without any
external functionality being affected.

Defragmentation is not implemented yet, but it is very unlikely to cause
problems until the OS grows to a *considerably* larger size.<br>
Again very easy to implement later if need be.


## Kmalloc
Usage is essentially identical to malloc().

Kmalloc's allocated size will actually be `num_of_bytes + HEADER_SIZE` rounded up to the minimum size of an allocation (currently 8 bytes).

The first `HEADER_SIZE` bytes are used to store the length of the memory in `BITMAP_PAGES` so that kfree can read it and know how much to free.


## Kfree
Usage mostly the same as free().<br>
Main exception is that a double free does not give an error and simply does nothing.

As mentioned before, kfree() gets number for the amount of data to free from a
header that is placed right before the pointer itself. So it will simply index
`ptr - HEADER_SIZE` to get it.

The memory is not set to 0 as this would be a waste of time. It just flags the memory as free to use.

There is no check for if you are giving it a valid pointer to free.
This is the callers responsibility.

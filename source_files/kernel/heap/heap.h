#pragma once
#include <stdint.h>

/*
 * Very simplistic implementation of malloc like heap allocation.
 * Instead of using arenas, this has one bitmap that represents the entire heap.
 *
 * Arenas are better especially when paging is in play, but this is
 * something that can be easily changed later internally without any
 * external functionality being affected.
 *
 *
 * Defragmentation isn't implemented yet, but it is very unlikely to cause
 * problems until the OS grows to a *considerably* larger size.
 * Again easy to implement later if need be.
 *
*/ 

// how many bytes of memory 1 bit represents
// Currently this is 
#define HEAP_BITMAP_PAGE_SIZE 8 
				
// This has to be divisible by the size of the bitmap page size
#define HEAP_SIZE	0xFBF72400

// Length of bitmap in bytes.
#define HEAP_BITMAP_LEN	HEAP_SIZE / HEAP_BITMAP_PAGE_SIZE / 8

// Kmalloc's allocated size will actually be n_bytes + header_size
// The first header_size bytes are used to store the length of the memory
// IN BITMAP PAGES so that kfree can read it and know how much to free.
//
// Currently is hardcoded and this macro is just used to increase readability.
// Do not change this without updating code.
#define HEAP_PTR_HEADER_SIZE	4

#define STACK_POS	0x90000 // Grows downward
#define HEAP_POS	0x90010 + HEAP_BITMAP_LEN // Grows upward
#define	HEAP_BITMAP_POS	0x90010

void *kmalloc(uint32_t n_bytes);
void kfree(uint8_t *ptr);

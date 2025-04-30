#pragma once
#include <stdint.h>

// how many bytes of memory 1 bit represents
// Currently this is 
#define HEAP_BITMAP_PAGE_SIZE 8 
				
// This has to be divisible by the size of the bitmap page size
#define HEAP_SIZE	0xFBF72400

// Length of bitmap in bytes.
#define HEAP_BITMAP_LEN	HEAP_SIZE / HEAP_BITMAP_PAGE_SIZE / 8

// Currently this is hardcoded and is just used to increase readability.
#define HEAP_PTR_HEADER_SIZE	4

#define STACK_POS	0x90000 // Grows downward
#define HEAP_POS	0x90010 + HEAP_BITMAP_LEN // Grows upward
#define	HEAP_BITMAP_POS	0x90010

extern uint8_t *g_heap_bitmap; // This is literally just a ptr to the bitmap.

void *kmalloc(uint32_t n_bytes);
void kfree(void *ptr);

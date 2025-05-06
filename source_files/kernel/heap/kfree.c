#include <heap.h>

// Change N bits starting from pos to 1
static inline void turn_off_bitmap_bits(uint32_t pos, uint32_t n)
{
	if (n == 0)
		return ;

	uint8_t	byte_pos = pos / 8;
	uint8_t	bit_pos = pos % 8;
	
	while (n > 0)
	{
		while (bit_pos < 8 && n > 0)
		{
			g_heap_bitmap[byte_pos] = \
				g_heap_bitmap[byte_pos] ^ (0x80 >> bit_pos);
			++bit_pos;
			--n;
		}
		bit_pos = 0;
		++byte_pos;
	}
}

static inline uint32_t get_page_count(uint8_t *ptr)
{
	return ( *((uint32_t*) (ptr - HEAP_PTR_HEADER_SIZE)) );
}

void kfree(void *ptr)
{
	// Pages to free
	uint32_t page_count = get_page_count((uint8_t*)ptr);

	// Get the bit position of the address on the bitmap
	uint32_t bit_pos = ((uint32_t) ptr - HEAP_POS - HEAP_PTR_HEADER_SIZE)
		/ HEAP_BITMAP_PAGE_SIZE;

	turn_off_bitmap_bits(bit_pos, page_count);
}

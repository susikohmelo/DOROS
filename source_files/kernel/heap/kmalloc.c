#include "heap.h"

uint8_t *g_heap_bitmap = (uint8_t*) HEAP_BITMAP_POS;

// REFER TO heap.h FOR MORE DETAILED DOCUMENTATION OF KMALLOC AND KFREE

// Return first free bitmap page that contains enough space to fit the value
// TODO needs optimizing.
static inline uint32_t find_free_memory(uint32_t n, uint8_t *error)
{
	uint32_t	start_bit_pos = 0;
	uint32_t	found_len = 0;	// Free bytes found thus far
	uint8_t		byte_pos = 0;	// Position of byte
	uint8_t		bit_pos = 0;	// Position within byte
					
	while (byte_pos < HEAP_BITMAP_LEN)
	{
		for (int8_t b = 7; b >= 0 && found_len < n; --b)
		{
			if ((g_heap_bitmap[byte_pos] >> b) & 0x01 != 0)
			{
				start_bit_pos = byte_pos + bit_pos + 1;
				bit_pos = 0;
				found_len = 0;
				break ;
			}
			++found_len;
			++bit_pos;
		}

		if (found_len >= n)
			break ;
		++byte_pos;
	}

	if (found_len < n)
		*error = 1;
	return start_bit_pos;
}

// Change N bits starting from pos to 1
static inline void turn_on_bitmap_bits(uint32_t pos, uint32_t n)
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
				g_heap_bitmap[byte_pos] | (0x80 >> bit_pos);
			++bit_pos;
			--n;
		}
		bit_pos = 0;
		++byte_pos;
	}
}

static inline void write_bitmap_header(uint32_t pos, uint32_t size)
{
	*((uint32_t*) (HEAP_POS + pos * HEAP_BITMAP_PAGE_SIZE)) = size;
}


void *kmalloc(uint32_t n_bytes)
{
	uint8_t	 find_error = 0;
	uint32_t page_count;
	uint32_t bit_pos;

	// Amount of bitmap pages to fit the data in, rounded up.
	page_count = (n_bytes + HEAP_PTR_HEADER_SIZE) / HEAP_BITMAP_PAGE_SIZE
		+ (((n_bytes + HEAP_PTR_HEADER_SIZE) % HEAP_BITMAP_PAGE_SIZE)
				!= 0);

	bit_pos = find_free_memory(page_count, &find_error);
	if (find_error)
		return 0;
	
	turn_on_bitmap_bits(bit_pos, page_count);

	// Bitpos will be incremented in the function
	write_bitmap_header(bit_pos, page_count);

	// Return the real address
	return ((void*) HEAP_POS + bit_pos * HEAP_BITMAP_PAGE_SIZE
			+ HEAP_PTR_HEADER_SIZE);
}

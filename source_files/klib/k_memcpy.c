#include <klib.h>

void	k_memcpy(void *s, void *d, uint32_t len)
{
	for (uint32_t i = 0; i < len; ++i)
	{
		*((uint8_t*)d) = *((uint8_t*)s);
		++d;
		++s;
	}
}

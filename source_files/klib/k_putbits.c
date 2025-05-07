#include <vga_tty.h>
#include <stdint.h>

void	k_terminal_putbits(uint8_t n)
{
	uint8_t res;
	for (uint8_t i = 1; i <= 8; ++i)
	{
		res = (n >> (8 - i)) & 0x01;
		terminal_putchar('0' + res);
	}
}

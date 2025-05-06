#include <vga_tty.h>
#include <stdint.h>

void	k_terminal_putnbr(int32_t n)
{
	if (n < 0)
	{
		n *= -1;
		terminal_putchar('-');
	}
	if (n / 10 > 0)
		k_terminal_putnbr(n / 10);
	terminal_putchar(n % 10 + '0');
}

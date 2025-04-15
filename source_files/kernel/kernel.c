#include "libk/include/vga_tty.h"

extern void main()
{
	terminal_init();

	terminal_putstring("You are now in the .C kernel!");

	while (1)
	{}

	return ;
}

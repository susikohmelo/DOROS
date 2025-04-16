#include "libk/include/vga_tty.h"

void main()
{
	terminal_init();

	//char str[] = "You are now in the .C kernel!";
	terminal_putstring("Hi");

	while (1)
	{}

	return ;
}

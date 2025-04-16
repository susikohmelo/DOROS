#include "libk/include/vga_tty.h"


// Each row is 80 wide even if it doesn't look like it
void print_bootup_message()
{
	// Store original color so we can change back to it before returning
	uint8_t original_color = get_color();

	// Color of the first half
	uint8_t color = vga_block_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
	terminal_setcolor(color);

	unsigned char *h1 = "\
00000000000000000000000000000000000000000000000000000000000000000000000000000000\
1                                                                              1\
2      oooooooooo.     .oooooo.   ooooooooo.     .oooooo.    .oooooo..o        2\
3      `888'   `Y8b   d8P'  `Y8b  `888   `Y88.  d8P'  `Y8b  d8P'    `Y8        3\
4       888      888 888      888  888   .d88' 888      888 Y88bo.             4\
5       888      888 888      888  888ooo88P'  888      888  `\"Y8888o.         5\
6       888      888 888      888  888`88b.    888      888      `\"Y88b        6";

	// Print first half
	terminal_putstring(h1);
	// Change color for second half
	color = vga_block_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_RED);
	terminal_setcolor(color);

	unsigned char *h2 = "\
7       888     d88' `88b    d88'  888  `88b.  `88b    d88' oo     .d8P        7\
8      o888bood8P'    `Y8bood8P'  o888o  o888o  `Y8bood8P'  8\"\"88888P'         8\
9                                                                              9\
10101010101010101010101010101010101010101010101010101010101010101010101010101010";
	terminal_putstring(h2);

	// Subtitle color
	color = vga_block_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
	terminal_setcolor(color);

	unsigned char *subtitle= "\
                           - Welcome to DOROS v 0.1 -                           ";
	terminal_putstring(subtitle);

	// Switch color back to the original color
	terminal_setcolor(original_color);
}

void main()
{
	terminal_init();
	print_bootup_message();

	while (1)
	{}
}

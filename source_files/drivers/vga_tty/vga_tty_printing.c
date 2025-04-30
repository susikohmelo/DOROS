// These headers are part of the GCC compiler - not the standard library.
// They work in standalone environments such as this one.
#include <stdint.h>

#include "../include/vga_tty.h"

// Please note! The cursor is not visually shown in any way.
// Any visual effects of the cursor will have to be done by the caller.
uint8_t		g_terminal_cursor_x;
uint8_t		g_terminal_cursor_y;
uint8_t		g_terminal_color;
uint16_t	*g_terminal_buffer;

// These may be useful for the aforementioned visuals
const uint8_t get_cursor_x(void)
{
	return (g_terminal_cursor_x);
}
const uint8_t get_cursor_y(void)
{
	return (g_terminal_cursor_y);
}
const uint8_t get_color(void)
{
	return (g_terminal_color);
}
void set_cursor_x(uint8_t x)
{
	if (x >= VGA_DEFAULT_WIDTH)
		x = VGA_DEFAULT_WIDTH - 1;
	g_terminal_cursor_x = x;
}
void set_cursor_y(uint8_t y)
{
	if (y >= VGA_DEFAULT_HEIGHT)
		y = VGA_DEFAULT_HEIGHT - 1;
	g_terminal_cursor_y = y;
}

// Combine individual colors for the foreground and backround together
uint8_t vga_block_color(uint8_t foreground, uint8_t background)
{
	return ( foreground | background << 4 );
}

// Combine a character and the colors together
static inline uint16_t vga_block(unsigned char c, uint8_t block_color)
{
	return ( (uint16_t) c | (uint16_t) block_color << 8 );
}

void terminal_setcolor(uint8_t color)
{
	g_terminal_color = color;
}

void terminal_putblock_at(unsigned char c, uint8_t color, uint8_t x, uint8_t y)
{
	g_terminal_buffer[VGA_DEFAULT_WIDTH * y + x] = vga_block(c, color);
}

// Scroll screen N rows upwards
void terminal_scrollup(uint8_t n)
{
	// Yes these are 16 bits on purpose.
	// For some reason the compiler does not increment with uint8 correctly
	uint16_t *dst;
	uint16_t src_row;
	uint16_t dst_row;
	for (uint8_t y = n; y < VGA_DEFAULT_HEIGHT; ++y)
	{
		dst_row = (y - n) * VGA_DEFAULT_WIDTH;
		src_row = y * VGA_DEFAULT_WIDTH;
		for (uint8_t x = 0; x < VGA_DEFAULT_WIDTH; ++x)
		{
			dst = &(g_terminal_buffer[dst_row + x]);
			*dst = g_terminal_buffer[src_row + x];
		}
	}

	// Clear N rows from the bottom of the screen
	const uint16_t clear_block = vga_block(' ', g_terminal_color);
	for (uint8_t y = VGA_DEFAULT_HEIGHT-1; y >= VGA_DEFAULT_HEIGHT - n; --y)
	{
		dst_row = y * VGA_DEFAULT_WIDTH;
		for (uint8_t x = 0; x < VGA_DEFAULT_WIDTH; ++x)
		{
			g_terminal_buffer[dst_row + x] = clear_block;
		}
	}
}

// Remove char from screen *from the current row only*
void terminal_removechar()
{
	if (g_terminal_cursor_y == 0 && g_terminal_cursor_x == 0)
		return ;

	if (g_terminal_cursor_x > 0)
		--g_terminal_cursor_x;
	else
	{
		g_terminal_cursor_x = VGA_DEFAULT_WIDTH - 1;
		--g_terminal_cursor_y;
	}

	terminal_putblock_at(' ', g_terminal_color,
				g_terminal_cursor_x, g_terminal_cursor_y);
}

// Put character and increment cursor
void terminal_putchar(unsigned char c)
{
	if (c == '\n')
	{
		g_terminal_cursor_x = 0;
		
		if (g_terminal_cursor_y + 1 < VGA_DEFAULT_HEIGHT)
			++g_terminal_cursor_y;
		else
			terminal_scrollup(1);
		return ;
	}

	terminal_putblock_at(c, g_terminal_color,
			g_terminal_cursor_x, g_terminal_cursor_y);
	// Increment cursor and wrap around if it goes over the bounds
	if (++g_terminal_cursor_x >= VGA_DEFAULT_WIDTH)
	{
		g_terminal_cursor_x = 0;
		if (g_terminal_cursor_y + 1 < VGA_DEFAULT_HEIGHT)
			++g_terminal_cursor_y;
		else
			terminal_scrollup(1);
	}
}

void terminal_clear_screen(void)
{
	// Character (block) used to clear the screen with.
	const uint16_t clear_block = vga_block(' ', g_terminal_color);

	for (uint8_t y = 0; y < VGA_DEFAULT_HEIGHT; ++y)
	{
		const uint16_t row_indx = y * VGA_DEFAULT_WIDTH;
		for (uint8_t x = 0; x < VGA_DEFAULT_WIDTH; ++x)
		{
			g_terminal_buffer[row_indx + x] = clear_block;
		}
	}
	g_terminal_cursor_x = 0;
	g_terminal_cursor_y = 0;
}

// Takes a C-string aka. null terminated character array.
void terminal_putstring(const unsigned char *c)
{
	while (*c != '\0')
	{
		terminal_putchar(*c);
		++c;
	}
}
void terminal_puterror(const unsigned char *c)
{
	uint8_t color = g_terminal_color;
	terminal_setcolor(VGA_DEFAULT_ERR_P_COLOR);
	terminal_putstring("[ ERROR ]");
	terminal_setcolor(VGA_DEFAULT_ERR_T_COLOR);
	terminal_putchar(' ');
	while (*c != '\0')
	{
		terminal_putchar(*c);
		++c;
	}
	terminal_setcolor(color);
}

void terminal_init(void)
{
	g_terminal_buffer = (uint16_t*) VGA_DEFAULT_LOCATION;
	g_terminal_color = (uint8_t) VGA_DEFAULT_COLOR;
	g_terminal_cursor_x = 0;
	g_terminal_cursor_y = 0;

	terminal_clear_screen();
}

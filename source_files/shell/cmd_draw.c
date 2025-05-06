// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.

#include <stdbool.h>

static int16_t	mouse_x;
static int16_t	mouse_y;
static bool	l_click;
static bool	r_click;

void mouse_catcher(int16_t x, int16_t y, bool l, bool r)
{
	mouse_x = x;
	mouse_y = y;
	l_click = l;
	r_click = l;
}

static void cmd_draw(uint8_t *args)
{
	(void) args;

	set_mouse_function(mouse_catcher);
	terminal_clearscreen();
}

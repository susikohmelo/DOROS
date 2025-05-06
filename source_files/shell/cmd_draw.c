// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.

#include <klib.h>

# define MOUSE_MAX_X VGA_DEFAULT_WIDTH - 1
# define MOUSE_MAX_Y VGA_DEFAULT_HEIGHT - 1

# define MOUSE_SENS 0.1
# define MOUSE_X_SENS MOUSE_SENS
# define MOUSE_Y_SENS MOUSE_SENS / 3.2 // To account for chars not being square

static uint16_t	g_prev_char;
static uint8_t	g_prev_pos[2];

static int8_t	g_draw_color;
static float	g_mouse_x;
static float	g_mouse_y;
static bool		g_l_down;
static bool		g_r_down;

static bool		g_exit_draw = false;

void exit_draw(uint8_t key)
{
	if (key > 127)
		return ;
	int8_t translated_key = keycode_map[key];
	if (translated_key == KEY_ESCAPE)
		g_exit_draw = true;
}

void mouse_catcher(int16_t x, int16_t y, bool l, bool r)
{
	g_mouse_x += x * MOUSE_X_SENS;
	g_mouse_y -= y * MOUSE_Y_SENS;
	g_l_down = l;
	g_r_down = r;

	// Constrain mouse X/Y to the screen coordinates
	if (g_mouse_x < 0)
		g_mouse_x = 0;
	else if (g_mouse_x > MOUSE_MAX_X)
		g_mouse_x = MOUSE_MAX_X;
	if (g_mouse_y < 0)
		g_mouse_y = 0;
	else if (g_mouse_y > MOUSE_MAX_Y)
		g_mouse_y = MOUSE_MAX_Y;
}

static inline void write_color_banner()
{
	uint8_t og_color = get_color();

	uint8_t t_clr = 0;
	for (uint8_t i = 0; i < 16; ++i) // Print colors 0=16
	{
		terminal_setcolor( ((t_clr << 4) & 0xF0) | (t_clr & 0x0F) );
		++t_clr;
		terminal_putstring("     ");
	}

	terminal_setcolor(og_color);
	terminal_putchar('\n');
	terminal_putstring("Press ESC to quit. Click above colors to choose one\n");
	terminal_putstring("You may draw over this message.");
}

static inline void show_mouse_on_screen()
{
		((uint16_t*) VGA_DEFAULT_LOCATION)[VGA_DEFAULT_WIDTH
			* g_prev_pos[1] + g_prev_pos[0]] = g_prev_char;

		// Store new character
		g_prev_pos[0] = (uint8_t) g_mouse_x;
		g_prev_pos[1] = (uint8_t) g_mouse_y;
		g_prev_char = ((uint16_t*) VGA_DEFAULT_LOCATION)[VGA_DEFAULT_WIDTH
			* g_prev_pos[1] + g_prev_pos[0]];

		// Put new cursor
		uint8_t new_c = g_prev_char >> 8; // Color of background char
		new_c ^= 0x0F;
		terminal_putblock_at('o', new_c,
				(uint8_t) g_mouse_x, (uint8_t) g_mouse_y);
}

static inline void choose_color()
{
	uint8_t t_clr = (((uint8_t) g_mouse_x) / 5);
    g_draw_color = ((t_clr << 4) & 0xF0) | (t_clr & 0x0F);
}

static inline void init_globals()
{
	g_exit_draw = 0;
	g_prev_char = ' ';
	g_draw_color = 0xFF;
	g_mouse_x = 0;
	g_mouse_y = 0;
	g_l_down = false;
	g_r_down = false;
	g_prev_pos[0] = 0;
	g_prev_pos[1] = 0;
}

static void cmd_draw(uint8_t *args)
{
	__asm__ __volatile__ ("cli");

	set_keyboard_function(&exit_draw);
	set_mouse_function(&mouse_catcher);
	set_ignore_rows(0);
	disable_cursor();

	init_globals();
	terminal_clear_screen();
	write_color_banner();

	__asm__ __volatile__ ("sti");
	while (g_exit_draw == false)
	{
		__asm__ __volatile__ ("HLT");
		if (g_l_down)
		{
			if ((uint8_t) g_mouse_y == 0)
			{
				choose_color();
				show_mouse_on_screen();
				continue;
			}
			terminal_putblock_at(' ', g_draw_color,
				(uint8_t) g_mouse_x, (uint8_t) g_mouse_y);
		}
		show_mouse_on_screen();
	}

	// Restore previous functionality
	terminal_clear_screen();
	enable_cursor();
	set_ignore_rows(0);
	set_mouse_function(0);
	set_keyboard_function(&key_catcher);
}

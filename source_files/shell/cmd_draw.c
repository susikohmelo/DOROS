// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.

#include <klib.h>

# define MOUSE_MAX_X VGA_DEFAULT_WIDTH - 1
# define MOUSE_MAX_Y VGA_DEFAULT_HEIGHT - 1

# define CUR_B1 0xC0
# define CUR_B2 0xC1
# define CUR_B3 0xC2
# define CUR_B4 0xC3

# define MOUSE_SENS 0.1
# define MOUSE_X_SENS MOUSE_SENS
# define MOUSE_Y_SENS MOUSE_SENS / 2 // To account for chars not being square

static uint8_t g_cursor_bits[16] =
{
	0b01000000,
	0b01100000,
	0b01110000,
	0b01110000,
	0b01111000,
	0b01111100,
	0b01111100,
	0b01111110,
	0b01111111,
	0b01111100,
	0b01011110,
	0b00001110,
	0b00001110,
	0b00000100,
	0b00000000,
	0b00000000
};
									   

static uint16_t	g_prev_char;
static uint8_t	g_prev_pos[2];

static uint8_t	g_draw_color;
static float	g_mouse_x;
static float	g_mouse_y;
static bool	g_l_down;
static bool	g_r_down;
static uint8_t *g_fonts;

static bool	g_exit_draw = false;

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
	terminal_putstring("Left click to draw - Right click to flood fill\n");
	terminal_putstring("You may draw over this message.");
}

static void store_new_prev_character(bool wipe_chardata)
{
	g_prev_pos[0] = (uint8_t) g_mouse_x;
	g_prev_pos[1] = (uint8_t) g_mouse_y;
	g_prev_char = ((uint16_t*) VGA_DEFAULT_LOCATION)[VGA_DEFAULT_WIDTH
		* g_prev_pos[1] + g_prev_pos[0]];
	if (wipe_chardata)
		g_prev_char = g_prev_char & 0xFF00;
}

static inline void show_mouse_on_screen()
{
	((uint16_t*) VGA_DEFAULT_LOCATION)[VGA_DEFAULT_WIDTH
		* g_prev_pos[1] + g_prev_pos[0]] = g_prev_char;
	store_new_prev_character(0);

	k_memcpy(g_cursor_bits, g_fonts + CUR_B1 * 16, 16); 
	set_fonts(g_fonts);

	// Put new cursor
	uint8_t new_c = g_prev_char >> 8; // Color of background char
	new_c ^= 0x0F;
	terminal_putblock_at(CUR_B1, new_c,
			(uint8_t) g_mouse_x, (uint8_t) g_mouse_y);
}

static void flood_fill(uint8_t x, uint8_t y, uint8_t t_clr, bool original)
{
	if (y == 0 || x >= VGA_DEFAULT_WIDTH || y >= VGA_DEFAULT_HEIGHT)
		return ;

	uint8_t cur_clr = (((uint16_t*) VGA_DEFAULT_LOCATION)[VGA_DEFAULT_WIDTH
		* y + x]) >> 8;
	if (!original && (cur_clr == g_draw_color
			|| (cur_clr != t_clr && cur_clr != g_draw_color)))
		return ;
	terminal_putblock_at(' ', g_draw_color, x, y);
	flood_fill(x + 1, y, t_clr, 0);
	flood_fill(x - 1, y, t_clr, 0);
	flood_fill(x, y + 1, t_clr, 0);
	flood_fill(x, y - 1, t_clr, 0);
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
	get_fonts(g_fonts);
}

static void cmd_draw(uint8_t *args)
{
	g_fonts = kmalloc(4096);
	if (!g_fonts)
		return;
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
		__asm__ __volatile__ ("CLI");
		if (g_r_down && (uint8_t)g_mouse_y != 0)
		{
			flood_fill(g_mouse_x, g_mouse_y, g_prev_char >> 8, 1);
			store_new_prev_character(0);
		}
		if (g_l_down)
		{
			if ((uint8_t) g_mouse_y == 0)
			{
				choose_color();
				goto END;
			}
			show_mouse_on_screen();
			terminal_putblock_at(0xC0, g_draw_color,
				(uint8_t) g_mouse_x, (uint8_t) g_mouse_y);
			store_new_prev_character(1);
		}
		END:
			show_mouse_on_screen();
			__asm__ __volatile__ ("STI");
	}

	// Restore previous functionality
	terminal_clear_screen();
	enable_cursor();
	set_ignore_rows(0);
	set_mouse_function(0);
	set_keyboard_function(&key_catcher);
	kfree(g_fonts);
}

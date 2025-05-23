// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.

#include "font"
#include <klib.h>

# define MOUSE_MAX_X VGA_DEFAULT_WIDTH - 1
# define MOUSE_MAX_Y VGA_DEFAULT_HEIGHT - 1

# define CUR_B1 0xC0
# define CUR_B2 0xC1
# define CUR_B3 0xC2
# define CUR_B4 0xC3

# define MOUSE_SENS 0.07
# define MOUSE_X_SENS MOUSE_SENS
# define MOUSE_Y_SENS MOUSE_SENS / 3.2 // To account for chars not being square

static uint8_t g_cursor_b[16] =
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
	0b00000110,
	0b00000000,
	0b00000000
};
									   

static uint16_t	g_prev_char1;
static uint16_t	g_prev_char2;
static uint16_t	g_prev_char3;
static uint16_t	g_prev_char4;
static uint8_t	g_prev_pos[2];

static uint8_t	g_draw_color;
static uint8_t	g_draw_key;
static float	g_mouse_x;
static float	g_mouse_y;
static bool	g_l_down;
static bool	g_r_down;
static uint8_t *g_fonts;

static bool	g_exit_draw = false;

void draw_key_catcher(uint8_t key)
{
	if (key > 127)
		return ;
	int8_t translated_key = keycode_map[key];
	if (translated_key == KEY_ESCAPE)
		g_exit_draw = true;
	else if (translated_key < 0)
		return ;
	g_draw_key = (uint8_t)translated_key;
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
	terminal_putchar('\n'); terminal_putchar('\n');
	terminal_putstring("Press ESC to quit.\n");
	terminal_putstring("Left click to draw - Right click to flood fill\n\n");
	terminal_putstring("Left  click top row to choose main color\n");
	terminal_putstring("Right click top row to choose character color\n");
	terminal_putstring("Press a key to select the character to draw with.\n\n");
	terminal_putstring("You may draw over this message.");
}

static void store_new_prev_character()
{
	g_prev_pos[0] = (uint8_t) g_mouse_x;
	g_prev_pos[1] = (uint8_t) g_mouse_y;

	uint16_t old_char;

	// TOPL
	old_char = g_prev_char1;
	g_prev_char1 = ((uint16_t*) VGA_DEFAULT_LOCATION) \
	[VGA_DEFAULT_WIDTH * g_prev_pos[1] + g_prev_pos[0]];
	// Keep old char if new one is a cursor byte
	if ((uint8_t)g_prev_char1 == CUR_B1)
		g_prev_char1 = old_char;

	// TOPR
	old_char = g_prev_char2;
	if ((uint8_t) g_mouse_x < VGA_DEFAULT_WIDTH - 1)
		g_prev_char2 = ((uint16_t*) VGA_DEFAULT_LOCATION) \
		[VGA_DEFAULT_WIDTH * (g_prev_pos[1] + 0) + (g_prev_pos[0] + 1)];
	// Keep old char if new one is a cursor byte
	if ((uint8_t)g_prev_char2 == CUR_B2)
		g_prev_char2 = old_char;

	// BOTTOML
	old_char = g_prev_char3;
	if ((uint8_t) g_mouse_y < VGA_DEFAULT_HEIGHT - 1)
		g_prev_char3 = ((uint16_t*) VGA_DEFAULT_LOCATION) \
		[VGA_DEFAULT_WIDTH * (g_prev_pos[1] + 1) + (g_prev_pos[0] + 0)];
	// Keep old char if new one is a cursor byte
	if ((uint8_t)g_prev_char3 == CUR_B3)
		g_prev_char3 = old_char;

	// BOTTOMR
	old_char = g_prev_char4;
	if ((uint8_t) g_mouse_y < VGA_DEFAULT_HEIGHT - 1
	&& (uint8_t) g_mouse_x < VGA_DEFAULT_WIDTH - 1)
		g_prev_char4 = ((uint16_t*) VGA_DEFAULT_LOCATION) \
		[VGA_DEFAULT_WIDTH * (g_prev_pos[1] + 1) + (g_prev_pos[0] + 1)];
	// Keep old char if new one is a cursor byte
	if ((uint8_t)g_prev_char4 == CUR_B4)
		g_prev_char4 = old_char;
}

static void draw_image(uint8_t *img, int8_t offx, int8_t offy, uint8_t c)
{
	for (int8_t y = 0; y < 16; ++y) // Init with our character
		img[y] = (g_fonts + c * 16)[y];

	// Pastes the bitmap based on the coordinate.
	// The if else tree is just because the bitshift can't take negatives
	if (offy >= 0)
	{
		if (offx >= 0)
			for (int8_t y = 0; y < 16 - offy; ++y)
				img[y + offy] ^= (g_cursor_b[y] >> offx);
		else
			for (int8_t y = 0; y < 16 - offy; ++y)
				img[y + offy] ^= (g_cursor_b[y] << (offx * -1));
	}
	else
	{
		offy *= -1;
		if (offx >= 0)
			for (int8_t y = 0; y < 16 - offy; ++y)
				img[y] ^= (g_cursor_b[y + offy] >> offx);
		else
			for (int8_t y = 0; y < 16 - offy; ++y)
				img[y] ^= (g_cursor_b[y + offy] << (offx * -1));
	}
}

static inline void draw_cursor_ascii()
{
	// Get a value from 0-16 based from the decimal portion of the float
	int8_t sub_x = ((float)g_mouse_x - (int8_t)g_mouse_x) * 8.0;
	int8_t sub_y = ((float)g_mouse_y - (int8_t)g_mouse_y) * 16.0;
	uint8_t img[16]; 

	draw_image(img, sub_x, sub_y, g_prev_char1);
	k_memcpy(img, g_fonts + CUR_B1 * 16, 16); 

	draw_image(img, sub_x - 8, sub_y, g_prev_char2);
	k_memcpy(img, g_fonts + CUR_B2 * 16, 16); 

	draw_image(img, sub_x, sub_y - 16, g_prev_char3);
	k_memcpy(img, g_fonts + CUR_B3 * 16, 16); 

	draw_image(img, sub_x - 8, sub_y - 16, g_prev_char4);
	k_memcpy(img, g_fonts + CUR_B4 * 16, 16); 
												 
	set_fonts(g_fonts);
}

// Yoink the mouse characters off screen
static void lift_mouse_off()
{
	terminal_putblock_at(g_prev_char1, g_prev_char1 >> 8, \
		g_prev_pos[0], g_prev_pos[1]);
	terminal_putblock_at(g_prev_char2, g_prev_char2 >> 8, \
		g_prev_pos[0] + 1, g_prev_pos[1] + 0);
	terminal_putblock_at(g_prev_char3, g_prev_char3 >> 8, \
		g_prev_pos[0] + 0, g_prev_pos[1] + 1);
	terminal_putblock_at(g_prev_char4, g_prev_char4 >> 8, \
		g_prev_pos[0] + 1, g_prev_pos[1] + 1);

	store_new_prev_character();
}

// Slam the mouse characters on screen
static void put_mouse_down()
{
	// Put new cursor
	uint8_t new_c = g_prev_char1 >> 8; // Color of background char
	new_c ^= 0x0F;
	terminal_putblock_at(CUR_B1, new_c,
			(uint8_t) g_mouse_x, (uint8_t) g_mouse_y);

	new_c = g_prev_char2 >> 8;
	new_c ^= 0x0F;
	terminal_putblock_at(CUR_B2, new_c,
			(uint8_t) g_mouse_x + 1, (uint8_t) g_mouse_y);

	new_c = g_prev_char3 >> 8;
	new_c ^= 0x0F;
	terminal_putblock_at(CUR_B3, new_c,
			(uint8_t) g_mouse_x, (uint8_t) g_mouse_y + 1);

	new_c = g_prev_char4 >> 8;
	new_c ^= 0x0F;
	terminal_putblock_at(CUR_B4, new_c,
			(uint8_t) g_mouse_x + 1, (uint8_t) g_mouse_y + 1);
}

static inline void update_mouse_on_screen()
{
	lift_mouse_off();	// Yank the mouse characters off the screen
	draw_cursor_ascii();	// Update cursor bits on the fonts
	put_mouse_down();	// Put the mouse characters back on screen
}

static void flood_fill(uint8_t x, uint8_t y, uint8_t t_clr, bool original)
{
	if (y == 0 || x >= VGA_DEFAULT_WIDTH || y >= VGA_DEFAULT_HEIGHT)
		return ;

	t_clr &= 0xF0; // We only care about the background color here
	uint8_t cur_clr = ((((uint16_t*) VGA_DEFAULT_LOCATION) \
			[VGA_DEFAULT_WIDTH * y + x]) >> 8) & 0xF0;
	if (!original && (cur_clr == (g_draw_color & 0xF0)
			|| (cur_clr != t_clr && cur_clr != (g_draw_color & 0xF0))))
		return ;
	terminal_putblock_at(' ', g_draw_color, x, y);
	flood_fill(x + 1, y, t_clr, 0);
	flood_fill(x - 1, y, t_clr, 0);
	flood_fill(x, y + 1, t_clr, 0);
	flood_fill(x, y - 1, t_clr, 0);
}

static inline void choose_color(bool type)
{
	if (type == 0)
	{
		uint8_t t_clr = (((uint8_t) g_mouse_x) / 5);
		g_draw_color &= 0x0F; // Reset BG color
		g_draw_color |= ((t_clr << 4) & 0xF0);
	}
	else
	{
		uint8_t t_clr = (((uint8_t) g_mouse_x) / 5);
		g_draw_color &= 0xF0; // Reset FG color
		g_draw_color |= (t_clr & 0x0F);
	}
}

static inline void print_block_bits()
{
	uint8_t og_color = get_color();
	terminal_setcolor(0x0F);
	set_cursor_x(0);
	set_cursor_y(1);
	terminal_putstring("clr | char bits: ");
	terminal_setcolor(0x0A);
	k_terminal_putbits((uint8_t) (g_prev_char1 >> 8));
	terminal_setcolor(0x0F);
	terminal_putstring(" | ");
	terminal_setcolor(0x0B);
	k_terminal_putbits((uint8_t) (g_prev_char1));
	terminal_setcolor(og_color); 
}

static inline void init_globals()
{
	g_exit_draw = 0;
	g_prev_char1	= 0;
	g_prev_char2	= 0;
	g_prev_char3	= 0;
	g_prev_char4	= 0;
	g_draw_color	= 0xFF;
	g_draw_key	= 0;
	g_mouse_x	= 0;
	g_mouse_y	= 0;
	g_l_down	= false;
	g_r_down	= false;
	g_prev_pos[0]	= 0;
	g_prev_pos[1]	= 0;
	g_fonts = g_font_bitmap;
}

static void cmd_draw(uint8_t *args)
{
	__asm__ __volatile__ ("cli");

	uint8_t og_color = get_color();
	set_keyboard_function(&draw_key_catcher);
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
			lift_mouse_off();
			flood_fill(g_mouse_x, g_mouse_y, (g_prev_char1 >> 8), 1);
			store_new_prev_character();
			draw_cursor_ascii();
			put_mouse_down();
		}
		else if (g_r_down && (uint8_t)g_mouse_y == 0)
			choose_color(1);
		if (g_l_down)
		{
			if ((uint8_t) g_mouse_y == 0)
			{
				choose_color(0);
				goto END;
			}
			update_mouse_on_screen();
			terminal_putblock_at(g_draw_key, g_draw_color,
				(uint8_t) g_mouse_x, (uint8_t) g_mouse_y);
			store_new_prev_character();
		}
		END:
			update_mouse_on_screen();
			print_block_bits();

			__asm__ __volatile__ ("STI");
	}

	// Restore previous functionality
	enable_cursor();
	set_ignore_rows(0);
	set_mouse_function(0);
	set_keyboard_function(&key_catcher); // shell key catcher
	terminal_setcolor(og_color); 
	terminal_clear_screen();
}

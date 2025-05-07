// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.


#define VGA_BUF_SIZE VGA_DEFAULT_HEIGHT * VGA_DEFAULT_WIDTH
static uint8_t g_rainbow_stop = true;

void run_rainbow()
{
	for (uint16_t i = 0; i < VGA_BUF_SIZE; ++i)
	{
		((uint16_t *)VGA_DEFAULT_LOCATION)[i] = i;
	}
	while (1)
	{
		for (uint16_t i = 0; i < VGA_BUF_SIZE; ++i)
		{
			__asm__ __volatile__ ("pause");
			++((uint16_t *)VGA_DEFAULT_LOCATION)[i];
			if (g_rainbow_stop)
				return;
		}
	}
}

void exit_rainbow(uint8_t key)
{
	if (key > 127)
		return ;
	int8_t translated_key = keycode_map[key];
	if (translated_key == KEY_ESCAPE)
		g_rainbow_stop = true;
}

void enter_rainbow(uint8_t key)
{
	if (key > 127)
		return ;
	int8_t translated_key = keycode_map[key];
	if (translated_key == KEY_ENTER)
		g_rainbow_stop = false;
}

static void cmd_rainbow(uint8_t *args)
{
	terminal_clear_screen();
	disable_cursor();

	g_rainbow_stop = true;
	terminal_putstring("Press ENTER to start - ESC to stop");
	while (g_rainbow_stop)
	{
		asm("hlt");
		set_keyboard_function(&enter_rainbow);
	}
	set_keyboard_function(&exit_rainbow);
	run_rainbow();

	terminal_clear_screen();
	enable_cursor();
	set_keyboard_function(&key_catcher);
}

static inline void print_process(uint32_t a, uint32_t b, uint8_t operator,
				uint32_t iter, uint8_t og_color)
{
	terminal_setcolor(vga_block_color(VGA_COLOR_BROWN, VGA_COLOR_BLACK));
	terminal_putchar('[');
	k_terminal_putnbr(iter);
	terminal_putstring("] ");
	k_terminal_putnbr(a);
	terminal_putchar(' ');
	terminal_putchar(operator);
	terminal_putchar(' ');
	k_terminal_putnbr(b);
	terminal_putchar('\n');
	terminal_setcolor(og_color);
}

static void cmd_math(uint8_t *args)
{
	uint8_t	og_color = get_color(); // Terminal color
	if (args == 0 || *args == 0)
	{
		terminal_setcolor(vga_block_color(VGA_COLOR_BLACK,
					VGA_COLOR_WHITE));
		terminal_putstring("Add arguments delimited by spaces. ");
		terminal_putstring("Order of operations is *not* respected.\n");
		terminal_setcolor(og_color);
		terminal_putstring("Example:\n");
		terminal_putstring("math 1 + 41\n");
		return ;
	}
	if (*args < '0' && *args > '9' && *args != '-')
		goto error;

	uint8_t *og_args = args;
	uint8_t	operator = 0;
	int32_t	a = k_atoi(args); // Get first argument and update position
	while ((*args >= '0' && *args <= '9') || *args == '-')
		++args;
	int32_t	b = 0;
	int32_t	iter = 0;

	while (*args)
	{
		if (*args == ' ')
			{ ++args; continue ; }

		if (*args < '0' || *args > '9'
				|| (*args == '-' && args[1] == ' '))
		{
			operator = *args;
			++args;
			continue ;
		}
		b = k_atoi(args);
		while ((*args >= '0' && *args <= '9') || *args == '-')
			++args;
		if (operator == 0) // Number without operand
			goto error;
		print_process(a, b, operator, ++iter, og_color);
		switch (operator)
		{
			case '+':
				a += b;
				break ;
			case '-':
				a -= b;
				break ;
			case '*':
				a *= b;
				break ;
			case '/':
				a /= b;
				break ;
			default:
				goto error;
		}
		operator = 0;
	}

	if (operator != 0)
		goto error;

	terminal_setcolor(vga_block_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK));
	terminal_putstring(og_args);
	terminal_putstring(" = ");
	k_terminal_putnbr(a);
	terminal_putchar('\n');
	terminal_setcolor(og_color);
	return ;

	error:
		terminal_setcolor(vga_block_color(VGA_COLOR_BLACK,
					VGA_COLOR_RED));
		terminal_puterror("Invalid input\n");
		terminal_setcolor(og_color);
}

static void cmd_ls(uint8_t *args)
{
	terminal_putstring("32-bit mode file system is not implemented yet\n");
}
static void cmd_clear(uint8_t *args)
{
	terminal_clear_screen();
}
static void cmd_help(uint8_t *args)
{
	uint8_t	og_color = get_color(); // Terminal color
	terminal_setcolor(vga_block_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
	terminal_putstring("Format: command arg1 arg2 ...\n");
	terminal_setcolor(og_color);

	// I'm sorry it looks funny, but this is the easiest way to format
	terminal_putstring("\
Commands:\n\
- help\n\
- clear   - clear screen\n\
- rainbow - epilepsy warning!\n\
- draw    - draw on screen with your mouse\n\
- math    - simple math operations\n\
- ls      - list current directory\n");
}

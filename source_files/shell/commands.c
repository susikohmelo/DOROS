// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.


// TODO move this into libk later
static inline void	k_terminal_putnbr(int32_t n)
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

static inline int32_t k_atoi(const uint8_t *nptr)
{
	int32_t	resulting_int;
	int8_t	sign;

	resulting_int = 0;
	sign = 1;
	while (*nptr && (*nptr == ' ' || *nptr == '\t' || *nptr == '\r'
			|| *nptr == '\n' || *nptr == '\v' || *nptr == '\f'))
		++nptr;
	if (*nptr == '-' || *nptr == '+')
	{
		if (*nptr == '-')
			sign = -1;
		++nptr;
	}
	while (*nptr >= '0' && *nptr <= '9')
	{
		resulting_int = resulting_int * 10 + *nptr - '0';
		++nptr;
	}
	return (resulting_int * sign);
}

#define VGA_BUF_SIZE VGA_DEFAULT_HEIGHT * VGA_DEFAULT_WIDTH
static uint8_t g_rainbow_stop = false;

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

void exit_rainbow(uint8_t stop)
{
	g_rainbow_stop = true;
}

static void cmd_rainbow(uint8_t *args)
{
	// Store characters on screen ---------------------------------------
	uint8_t		old_x = get_cursor_x();
	uint8_t		old_y = get_cursor_y();
	uint16_t	*old_buf;
	old_buf = kmalloc(VGA_BUF_SIZE * 2);
	if (!old_buf)
		return ;
	for (uint16_t i = 0; i < VGA_BUF_SIZE; ++i)
	{
		old_buf[i] = ((uint16_t *)VGA_DEFAULT_LOCATION)[i];
	}
	terminal_clear_screen();

	g_rainbow_stop = false;
	set_keyboard_function(&exit_rainbow);
	run_rainbow();

	// Restore characters on screen ------------------------------------
	set_cursor_x(old_x);
	set_cursor_y(old_y);
	for (uint16_t i = 0; i < VGA_DEFAULT_HEIGHT * VGA_DEFAULT_WIDTH; ++i)
	{
		((uint16_t *)VGA_DEFAULT_LOCATION)[i] = old_buf[i];
	}
	kfree(old_buf);
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
					VGA_COLOR_LIGHT_GRAY));
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
	terminal_setcolor(vga_block_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GRAY));
	terminal_putstring("Format: command arg1 arg2 ...\n");
	terminal_setcolor(og_color);

	// I'm sorry it looks funny, but this is the easiest way to format
	terminal_putstring("\
Commands:\n\
- help\n\
- clear   - clear screen\n\
- rainbow - epilepsy warning!\n\
- math    - simple math operations\n\
- ls      - list current directory\n");
}

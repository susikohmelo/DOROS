// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.


// TODO move this into libk later
static inline void	k_terminal_putnbr(int16_t n)
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

static inline int8_t k_atoi(const uint8_t *nptr)
{
	int8_t	resulting_int;
	uint8_t	sign;

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

static void cmd_math(uint8_t *args)
{
	if (args == 0 || *args == 0)
	{
		terminal_putstring("Add arguments delimited by spaces. ");
		terminal_putstring("Integers only - overflow not checked.\n");
		terminal_putstring("Example:\n");
		terminal_putstring("math 1 + 41\n");
	}
	if (*args < '0' && *args > '9')
		goto error;

	uint8_t	operator = 0;
	int8_t	a = k_atoi(args); // Get first argument and update position
	while (*args >= '0' && *args <= '9')
		++args;
	int8_t	b = 0;

	while (*args)
	{
		if (*args == ' ')
			continue ;
		if (*args < '0' && *args > '9')
		{
			operator = *args;
			++args;
			continue ;
		}
		b = k_atoi(*args);
		while (*args >= '0' && *args <= '9')
			++args;
		if (operator == 0) // Number without operand
			goto error;
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

	terminal_putstring(" = ");
	k_terminal_putnbr(a);
	terminal_putchar('\n');
	return ;

	error:
		terminal_putstring("Invalid input\n");
		return ;
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
	// I'm sorry it looks funny, but this is the easiest way to format
	terminal_putstring("\
Format: command arg1 arg2 ...\n\n\
Commands:\n\
- help\n\
- clear\n\
- math\n\
- ls\n");
}

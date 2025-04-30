#include "../drivers/include/vga_tty.h"
#include "../drivers/include/keyboard.h"
#include "../kernel/heap/heap.h"
#include <stdint.h>
#include <stdbool.h>

#define SHELL_PROMPT_SIZE 17
#define SHELL_BUF_SIZE VGA_DEFAULT_WIDTH - SHELL_PROMPT_SIZE
#define SHELL_PROMPT_FG_COLOR VGA_COLOR_BLACK
#define SHELL_PROMPT_BG_COLOR VGA_COLOR_GREEN
#define SHELL_BAN_FG_COLOR VGA_COLOR_BLACK
#define SHELL_BAN_BG_COLOR VGA_COLOR_LIGHT_GRAY

uint8_t	g_ready_to_execute = false;
int8_t	g_keybuffer[SHELL_BUF_SIZE + 1]; // + 1 so it's always null terminated
uint8_t	g_keybuffer_len = 0;
int8_t	g_keybuffer_pos = 0;

uint8_t g_arrowkey_down = false;
uint8_t	g_shift_down = false;

void key_catcher(uint8_t keycode);
void k_terminal_putnbr(int32_t n);

// TODO move this into a libk later
static int8_t k_memcmp(void *s1, void *s2, uint8_t n)
{
	while ( *((int8_t*)s1) == *((int8_t*)s2) && n-- > 0 )
	{
		++s1;
		++s2;
	}
	return (*((int8_t*)s1) - *((int8_t*)s2));
}

static void flush_buffer()
{
	for (uint8_t i = 0; i < SHELL_BUF_SIZE; ++i)
		g_keybuffer[i] = 0;
	g_keybuffer_len = 0;
	g_keybuffer_pos = 0;
}

static void write_banner()
{
	uint8_t og_x = get_cursor_x();
	uint8_t og_y = get_cursor_y();
	set_cursor_x(0); set_cursor_y(0); // Store old xy and set new to 0
					  
	uint8_t	og_color = get_color();
	uint8_t	ban_color = vga_block_color(SHELL_BAN_FG_COLOR,
					SHELL_BAN_BG_COLOR);
	uint8_t	true_color = vga_block_color(VGA_COLOR_GREEN,
					SHELL_BAN_BG_COLOR);
	uint8_t	false_color = vga_block_color(VGA_COLOR_RED,
					SHELL_BAN_BG_COLOR);

	terminal_setcolor(ban_color); // Flush the first row of characters
	for (uint8_t i = 0; i < VGA_DEFAULT_WIDTH; ++i)
		terminal_putchar(' ');
	set_cursor_y(0);

	terminal_putstring(" X: ");
	k_terminal_putnbr(g_keybuffer_pos);
	set_cursor_x(8);
	terminal_putstring("Y: ");
	k_terminal_putnbr(og_y);
	set_cursor_x(8 + 10);

	terminal_putstring("buffer-len: ");
	k_terminal_putnbr(g_keybuffer_len);

	set_cursor_x(8 + 10 + 14 + 6);
	terminal_putstring("shift: ");
	if (g_shift_down)
	{
		terminal_setcolor(true_color);
		terminal_putstring("true");
	}
	else
	{
		terminal_setcolor(false_color);
		terminal_putstring("false");
	}

	terminal_setcolor(og_color);
	set_cursor_x(og_x); set_cursor_y(og_y);
}

static void write_prompt()
{
	if (get_cursor_y() == 0)
		set_cursor_y(1);
	uint8_t	og_color = get_color(); // Terminal color
	terminal_setcolor(vga_block_color(SHELL_PROMPT_FG_COLOR,
					SHELL_PROMPT_BG_COLOR));
	terminal_putstring("[ PICOSHELL ]");
	terminal_setcolor(og_color);
	terminal_putstring(" > ");
}

#include "commands.c" // cmd_help etc.

static inline void invalid_command()
{
	terminal_puterror("Unknown command\n");
}

// Don't we all love function pointer syntax in C... No? Too bad.
// This takes a string, and returns a function pointer.
static inline void (*get_function_ptr(uint8_t *cmd))(uint8_t*)
{
	// Yes, hashmaps would be neat but unnecessary as we have few commands
	if (k_memcmp(cmd, "help", 4) == 0)
		return &cmd_help;
	if (k_memcmp(cmd, "ls", 2) == 0)
		return &cmd_ls;
	if (k_memcmp(cmd, "clear", 5) == 0)
		return &cmd_clear;
	if (k_memcmp(cmd, "math", 4) == 0)
		return &cmd_math;
	if (k_memcmp(cmd, "rainbow", 7) == 0)
		return &cmd_rainbow;
	return 0;
}

// The shell only takes 1 command + arguments, no funny stuff like pipes
// So we can just do a very lazy left to right parsing.
void execute_buffer()
{
	g_ready_to_execute = false;
	terminal_putchar('\n'); // Newline regardless of what happens next
				
	if (g_keybuffer[0] == 0) // Empty cmd
	{
		write_prompt();
		return ;
	}

	uint8_t pos = 0;
	uint8_t *cmd;

	// Increment until end of buffer or space
	while (g_keybuffer[pos] != 0 && g_keybuffer[pos] != ' ')
		++pos;

	// Basically just strduping the command
	cmd = kmalloc(pos + 1);
	if (cmd == 0)
		return ;

	for (uint8_t i = 0; i < pos; ++i)
		cmd[i] = g_keybuffer[i];
	cmd[pos] = 0;

	void (*fun)(uint8_t*) = get_function_ptr(cmd);
	if (fun == 0) // Invalid command
	{
		invalid_command();
		flush_buffer();
		write_prompt();
		kfree(cmd);
		return ;
	}
	fun(g_keybuffer + pos + 1 * (pos < SHELL_BUF_SIZE - 1));
	flush_buffer();
	write_prompt();
	kfree(cmd);
}

static void shell_move_cursor(int8_t dir)
{
	uint8_t x = get_cursor_x();
	if (g_keybuffer_pos <= 0
			|| x + dir >= SHELL_PROMPT_SIZE + g_keybuffer_len) 
		return ;
	set_cursor_x(x + dir);
	g_keybuffer_pos += dir;
}

static void handle_normal_input(int8_t key) // Normal keys are just stored
{
	if (g_keybuffer_len >= SHELL_BUF_SIZE)
		return ;
	if (g_arrowkey_down) // Input is an arrowkey press
	{
		switch (key)
		{
			case '2': // down - ignored
				return ;
			case '4': // left
				shell_move_cursor(-1); return ;
			case '6': // right
				shell_move_cursor(1); return  ;
			case '8': // up - ignored
				return ;
		}
		return ;
	}
	if (g_shift_down)
	{
		if (key >= 'a' && key <= 'z')
			key -= ('a' - 'A');
		else
		{
			switch (key)
			{
				case '0':
					key = ')'; break ;
				case '1':
					key = '!'; break ;
				case '2':
					key = '@'; break ;
				case '3':
					key = '#'; break ;
				case '4':
					key = '$'; break ;
				case '5':
					key = '%'; break ;
				case '6':
					key = '^'; break ;
				case '7':
					key = '&'; break ;
				case '8':
					key = '*'; break ;
				case '9':
					key = '('; break ;
				case '-':
					key = '_'; break ;
				case '=':
					key = '+'; break ;
			}
		}
	}
	g_keybuffer[g_keybuffer_pos++] = key;
	g_keybuffer_len = g_keybuffer_pos > g_keybuffer_len \
		? g_keybuffer_pos : g_keybuffer_len;
	terminal_putchar(key);
}

static void handle_special_input(int8_t key) // Special keys are like commands
{
	switch (key) // switch is overkill but this list will grow with time
	{
		case KEY_BACKSPACE:
			if (g_keybuffer_pos > 0)
			{
				if (g_keybuffer_pos >= g_keybuffer_len)
					--g_keybuffer_len;
				g_keybuffer[g_keybuffer_pos--] = ' ';
				terminal_removechar();
			}
			return ;
		case KEY_ENTER:
			g_ready_to_execute = true;
			return ;
		case KEY_LEFT_SHIFT:
			g_shift_down = true;
			return ;
	}
}

void key_catcher(uint8_t keycode) // Async function called on keyboard interrupt
{
	if (keycode > 127) // Beyond mapped input range
	{
		if (keycode == 224) // This comes before a direction key
		{
			if (g_arrowkey_down)
				g_arrowkey_down = false;
			else
				g_arrowkey_down = true;
		}
		else if (keycode == 0xAA) // Shift release key
			g_shift_down = false;
		return ;
	}

	int8_t		translated_key = keycode_map[keycode];
	if (translated_key > 0) // Normal character pressed
	{
		handle_normal_input(translated_key);
		return ;
	}
	else
	{
		handle_special_input(translated_key);
		return ;
	}
}

static void launch_message()
{
	uint8_t	og_color = get_color(); // Terminal color
	terminal_setcolor(vga_block_color(VGA_COLOR_LIGHT_GRAY,
					VGA_COLOR_RED));
	terminal_putstring("\
You are now in PICOSHELL - Type 'help' for a list of commands.                  ");
	terminal_setcolor(og_color);
}

void launch_picoshell()
{
	flush_buffer();
	enable_cursor();
	set_keyboard_function(&key_catcher); // Redirect keyboard input here
	terminal_putchar('\n');
	launch_message();
	write_prompt();

	uint8_t	active_arrow_counter = 0; // Sometimes the arrow key doesn't
					  // send the release signal.
					  // This is a timer to switch it off
					  // if it doesnt
	loop:
		__asm__ __volatile__ ("hlt");
		if (g_ready_to_execute == true)
			execute_buffer();
		write_banner(); // Update key-info
		if (g_arrowkey_down)
		{
			++active_arrow_counter;
			if (active_arrow_counter > 1)
				active_arrow_counter = g_arrowkey_down = false;
		}
		goto loop;
}

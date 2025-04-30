#include "../drivers/include/vga_tty.h"
#include "../drivers/include/keyboard.h"
#include "../kernel/heap/heap.h"
#include <stdint.h>
#include <stdbool.h>

#define SHELL_PROMPT_SIZE 17
#define SHELL_BUF_SIZE VGA_DEFAULT_WIDTH - SHELL_PROMPT_SIZE
#define SHELL_PROMPT_FG_COLOR VGA_COLOR_BLACK
#define SHELL_PROMPT_BG_COLOR VGA_COLOR_GREEN

uint8_t	g_ready_to_execute = false;
int8_t	g_keybuffer[SHELL_BUF_SIZE + 1]; // + 1 so it's always null terminated
uint8_t	g_keybuffer_len = 0;
uint8_t	g_shift_down = 0;

void key_catcher(uint8_t keycode);

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
}

static void write_prompt()
{
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

static void handle_normal_input(int8_t key) // Normal keys are just stored
{
	if (g_keybuffer_len >= SHELL_BUF_SIZE)
		return ;
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
		g_shift_down = false;
	}
	g_keybuffer[g_keybuffer_len++] = key;
	terminal_putchar(key);
}

static void handle_special_input(int8_t key) // Special keys are like commands
{
	switch (key) // switch is overkill but this list will grow with time
	{
		case KEY_BACKSPACE:
			if (g_keybuffer_len > 0)
			{
				--g_keybuffer_len;
				g_keybuffer[g_keybuffer_len] = 0;
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
	static bool	is_arrowkey = false;
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
	terminal_setcolor(vga_block_color(VGA_COLOR_WHITE,
					VGA_COLOR_RED));
	terminal_putstring("\
You are now in PICOSHELL - Type 'help' for a list of commands.                  ");
	terminal_setcolor(og_color);
}

void launch_picoshell()
{
	flush_buffer();
	launch_message();
	enable_cursor();
	set_keyboard_function(&key_catcher); // Redirect keyboard input here
	write_prompt();

	loop:
		__asm__ __volatile__ ("hlt");
		if (g_ready_to_execute == true)
			execute_buffer();
		goto loop;
}

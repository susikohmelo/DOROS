#include "../drivers/include/vga_tty.h"
#include "../drivers/include/keyboard.h"
#include "../kernel/heap/heap.h"
#include <stdint.h>
#include <stdbool.h>

#define SHELL_PROMPT_SIZE 17
#define SHELL_BUF_SIZE VGA_DEFAULT_WIDTH - SHELL_PROMPT_SIZE
#define SHELL_PROMPT_FG_COLOR VGA_COLOR_BLACK
#define SHELL_PROMPT_BG_COLOR VGA_COLOR_GREEN

int8_t	g_keybuffer[SHELL_BUF_SIZE + 1]; // + 1 so it's always null terminated
uint8_t	g_keybuffer_len = 0;

// TODO move these into libk later
static int8_t k_memcmp(void *s1, void *s2, uint8_t n)
{
	while ( *((int8_t*)s1) == *((int8_t*)s2) && n-- > 0 )
	{
		++s1;
		++s2;
	}
	return (*((int8_t*)s1) - *((int8_t*)s2));
}
static uint8_t k_strlen(uint8_t *str)
{
	uint8_t	len = 0;

	while (str[len] != 0)
		++len;
	return len;
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

static void cmd_ls(uint8_t *args)
{
	terminal_putstring("32-bit mode file system is not implemented yet\n");
}

static void cmd_clear(uint8_t *args)
{
	terminal_clear_screen();
}

static inline void invalid_command()
{
	terminal_putstring("Unknown command\n");
}

// Don't we all love function pointer syntax in C... No? Too bad.
// This takes a string, and returns a function pointer.
static inline void (*get_function_ptr(uint8_t *cmd))(uint8_t*)
{
	// Yes, hashmaps would be neat but unnecessary as we have few commands
	if (k_memcmp(cmd, "ls", 2) == 0)
		return &cmd_ls;
	if (k_memcmp(cmd, "clear", 5) == 0)
		return &cmd_clear;
	return 0;
}

// The shell only takes 1 command + arguments, no funny stuff like pipes
// So we can just do a very lazy left to right parsing.
void execute_buffer()
{
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

	void (*f)(uint8_t*) = get_function_ptr(cmd);
	if (f == 0) // Invalid command
	{
		invalid_command();
		flush_buffer();
		write_prompt();
		kfree(cmd);
		return ;
	}
	// TODO argument parsing should go here
	f(0);
	flush_buffer();
	write_prompt();
	kfree(cmd);
}

static void handle_normal_input(int8_t key) // Normal keys are just stored
{
	if (g_keybuffer_len >= SHELL_BUF_SIZE)
		return ;
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
			execute_buffer();
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

void launch_picoshell()
{
	flush_buffer();
	set_keyboard_function(&key_catcher); // Redirect keyboard input here
	write_prompt();

	while (1) // TODO inefficient to do this
	{}
}

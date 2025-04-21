#include "../drivers/include/vga_tty.h"
#include "../drivers/include/keyboard.h"
#include "../kernel/heap/heap.h"
#include <stdint.h>
#include <stdbool.h>

void key_catcher(uint8_t keycode) // Async function called on keyboard interrupt
{
	static	is_arrowkey = false;
	int8_t	translated_key = keycode_map[keycode];

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
}

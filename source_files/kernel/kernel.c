
/*
 *
 * This is the main kernel file.
 * It contains the primary abstract logic.
 * 
 *
 * As much as possible the functions are contained within their own files and 
 * we are just calling them in order here.
 *
*/

#include "../drivers/include/vga_tty.h"
#include "../drivers/include/keyboard.h"
#include "interrupts/interrupt_utils.h"
#include "interrupts/IDT.h"
#include "heap/heap.h"
#include <stdint.h>


// Don't bother making a header just for this
void print_bootup_message();

// Very simple function to just test we're getting input.
void test_keyinput(uint8_t keycode)
{
	int8_t	translated_key = keycode_map[keycode];
	if (translated_key < 0) // special key
		return ;
	terminal_putchar(translated_key);
}

void main()
{
	terminal_init();

	init_IDT();
	init_keyboard();
	set_keyboard_function(&test_keyinput);

	print_bootup_message();
	enable_interrupts(); // Re-enable interrupts

	// Testing kmalloc/free
	uint8_t *ptr = kmalloc(1);
	*ptr = 2;
	kfree(ptr);

	while (1) // Not optimal, just for testing
	{}
}

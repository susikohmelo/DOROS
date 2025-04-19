
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
#include <stdint.h>


// Don't bother making a header just for this
void print_bootup_message();

// Very simple function to just test we're getting input. No they printed
// values are not mapped correctly to the keys we just wanna see something.
void test_keyinput(uint8_t keycode)
{
	terminal_putchar(keycode + '0');
}

void main()
{
	terminal_init();

	init_IDT();
	init_keyboard();
	set_keyboard_function(&test_keyinput);

	print_bootup_message();
	enable_interrupts(); // Re-enable interrupts

	while (1) // Not optimal, just for testing
	{}
}

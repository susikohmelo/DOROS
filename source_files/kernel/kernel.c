
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
#include "../drivers/include/mouse.h"
#include "interrupts/interrupt_utils.h"
#include "interrupts/IDT.h"
#include "heap/heap.h"
#include <stdbool.h>
#include <stdint.h>


// Don't bother making a header just for this
void print_bootup_message();
void launch_picoshell();

void main()
{
	terminal_init();
	disable_cursor();

	init_IDT();
	init_keyboard();
	init_mouse(); //NOT FULLY IMPLEMENTED!

	print_bootup_message();
	enable_interrupts(); // Re-enable interrupts

	asm("HLT"); // Wait for any input

	terminal_clear_screen();
	launch_picoshell();

	while (1) // Not optimal, just for quick testing.
	{}
}

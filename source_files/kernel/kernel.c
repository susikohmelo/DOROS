
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
#include <stdbool.h>
#include <stdint.h>


// Don't bother making a header just for this
void print_bootup_message();
void launch_picoshell();

// This function is used just so that the keyboard interrupt has some sort of
// memory address to jump to.
void keyboard_init_function(uint8_t keycode) {}


void main()
{
	terminal_init();
	disable_cursor();

	init_IDT();
	init_keyboard();

	print_bootup_message();
	enable_interrupts(); // Re-enable interrupts

	set_keyboard_function(&keyboard_init_function); // Wait for input
	asm("HLT");

	terminal_clear_screen();
	launch_picoshell();

	while (1) // Not optimal, just for quick testing.
	{}
}

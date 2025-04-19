#include "../../kernel/interrupts/IDT.h"
#include "../include/vga_tty.h"

// ASM function that gets called before the real handler
extern void	recieve_keyboard_interrupt();

void init_keyboard()
{
	// The IDT is just an array of entries.
	struct IDT_entry *IDT = get_IDT();

	// This is the memory address of the function
	uint32_t	offset = (uint32_t)recieve_keyboard_interrupt;

	// Interrupts 0-32 are reserved for the CPU. 33 is the first free one.
	IDT[33].offset_lowerbits = offset & 0x0000FFFF; // Lower half
	IDT[33].selector = KERNEL_SEGMENT_OFFSET;
	IDT[33].zero = 0;
	IDT[33].type_attribute = IDT_INTERRUPT_GATE;
	IDT[33].offset_upperbits = (offset & 0xFFFF0000) >> 16; // Higher half
								  //
	/*
	* Unmask the keyboard IRQ, which is 1
	* 0xFD = 1111 1101
	* From right to left the ports are 0-7.
	* So we are setting port 1 as 0, aka not masked.
	*/
	ioport_out(PIC1_DATA_PORT, 0xFD);
	// TODO this might overwrite the mask of another driver
}

// This gets called on keyboard interrupts
void handle_keyboard_interrupt()
{
	// Reset command - yank that interrupt off the queue
	ioport_out(PIC1_CMD_PORT, 0x20);

	// Yoink the status
	uint8_t status = ioport_in(KEYBOARD_STATUS_PORT);

	if (status & 0x1) // If a new key has been pressed
	{
		uint8_t keycode = ioport_in(KEYBOARD_DATA_PORT);
		if (keycode < 0 || keycode >= 128) // Outside our mapped keys
			return ;
		// TODO this is just for testing.
		// This needs to probably be thrown in to some sort of buffer.
		// Maybe a specific memory location where a buffer of inputs is
		// stored and a getter function for running processes to be
		// able to poll the inputs?
		terminal_putchar(keycode + '0');
		
	}
}

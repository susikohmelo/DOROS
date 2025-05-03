#include "../../kernel/interrupts/IDT.h"
#include "../include/mouse.h"

// ASM function that gets called before the real handler
extern void	recieve_mouse_interrupt();

uint8_t	g_keybuffer_size = 0; // Amount of items in the buffer
void (*g_mouse_function)(uint32_t) = 0;

void set_mouse_function(void (*f)(uint32_t))
{
	g_mouse_function = f;
}

void init_mouse()
{
	// The IDT is just an array of entries.
	struct IDT_entry *IDT = get_IDT();

	// This is the memory address of the function
	uint32_t	offset = (uint32_t)recieve_mouse_interrupt;

	// Interrupts 0-32 are reserved for the CPU. 33 is the first free one.
	// 33 is reserved for the keyboard. So 34 shall do.
	IDT[34].offset_lowerbits = offset & 0x0000FFFF; // Lower half
	IDT[34].selector = KERNEL_SEGMENT_OFFSET;
	IDT[34].zero = 0;
	IDT[34].type_attribute = IDT_INTERRUPT_GATE;
	IDT[34].offset_upperbits = (offset & 0xFFFF0000) >> 16; // Higher half

	/*
	* Unmask the mouse IRQ, which is 2
	* 0xFB = 1110 1011
	* From right to left the ports are 0-7.
	* So we are setting port 1 as 0, aka not masked.
	*/
	uint8_t mask = ioport_in(PIC2_DATA_PORT); // Get current mask
	mask &= 0xFB;
	ioport_out(PIC2_DATA_PORT, mask);
}

// This gets called on mouse interrupts
void handle_mouse_interrupt()
{
	// Reset command - yank that interrupt off the queue
	ioport_out(PIC2_CMD_PORT, 0x20);

	// Yoink the status
	uint32_t status = ioport_in(MOUSE_STATUS_PORT);

	if (status & 0x1) // If there is a new input to be had
	{
		uint32_t x = ioport_in(MOUSE_DATA_PORT);
		if (g_mouse_function == 0)
			return ;
		g_mouse_function(x);
	}
}

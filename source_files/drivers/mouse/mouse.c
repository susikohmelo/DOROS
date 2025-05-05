#include "../../kernel/interrupts/IDT.h"
#include "../include/mouse.h"

// ASM function that gets called before the real handler
extern void	recieve_mouse_interrupt();

void (*g_mouse_function)(uint32_t) = 0;

void set_mouse_function(void (*f)(uint32_t))
{
	g_mouse_function = f;
}

void wait_for_mouse(uint8_t poll_type)
{
	uint32_t max_iterations = 1000000;

	if (poll_type == 0)
	{
		while (--max_iterations)
		{
			__asm__ __volatile__ ("pause");
			if ((ioport_in(0x64) & 1) == 1)
				break;
		}
	}
	else
	{
		while (--max_iterations)
		{
			__asm__ __volatile__ ("pause");
			if ((ioport_in(0x64) & 2) == 0)
				break;
		}
	}
	return;
}


static void write_to_mouse(uint8_t data)
{
	wait_for_mouse(1);
	ioport_out(0x64, 0xD4); // Tell the port we wanna talk to the mouse
	
	wait_for_mouse(1);
	ioport_out(0x60, data); // Give our boy the good news.
}

static uint8_t read_from_mouse()
{
	wait_for_mouse(0);
	return (ioport_in(0x60));
}

void init_mouse()
{
	uint8_t compaq;
	
	wait_for_mouse(1);
	ioport_out(0x64, 0xA8); // Enable AUX input

	// Get compaq status
	wait_for_mouse(1);
	ioport_out(0x64, 0x20);

	wait_for_mouse(0);
	compaq = (ioport_in(0x60) | 2); // Enable bit 1 (IRQ12 enable)
	// compaq &= 0b11011111; // Clear bit 5 (mouse clock)
	wait_for_mouse(1);
	ioport_out(0x64, 60);
	wait_for_mouse(1);
	ioport_out(0x60, compaq);

	write_to_mouse(0xF6);
	read_from_mouse();

	write_to_mouse(0xF4);
	read_from_mouse();

	// The IDT is just an array of entries.
	struct IDT_entry *IDT = get_IDT();

	// This is the memory address of the function
	uint32_t	offset = (uint32_t)recieve_mouse_interrupt;

	// Interrupts 0-32 are reserved for the CPU. 32 + 12 is our interrupt
	IDT[32 + 12].offset_lowerbits = offset & 0x0000FFFF; // Lower half
	IDT[32 + 12].selector = KERNEL_SEGMENT_OFFSET;
	IDT[32 + 12].zero = 0;
	IDT[32 + 12].type_attribute = IDT_INTERRUPT_GATE;
	IDT[32 + 12].offset_upperbits = (offset & 0xFFFF0000) >> 16; // Higher half

	/*
	* Unmask the mouse IRQ, which is 12 (starts from 8)
	* 0xEF = 1110 1111
	* From right to left the ports are 0-7.
	* So we are setting port 1 as 0, aka not masked.
	*/
	uint8_t mask = ioport_in(PIC2_DATA_PORT); // Get current mask
	//mask &= 0xEF;
	mask &= 0x00;
	ioport_out(PIC2_DATA_PORT, mask);

	// We also need to open up IRQ 2 on PIC1
	mask = ioport_in(PIC1_DATA_PORT); // Get current mask
	mask &= 0x01;
	ioport_out(PIC1_DATA_PORT, mask);
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

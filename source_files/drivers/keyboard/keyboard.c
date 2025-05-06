#include <IDT.h>
#include <keyboard.h>

// ASM function that gets called before the real handler
extern void	recieve_keyboard_interrupt();

uint8_t	g_keybuffer_size = 0; // Amount of items in the buffer
void (*g_keyboard_function)(uint8_t) = 0;

void set_keyboard_function(void (*f)(uint8_t))
{
	g_keyboard_function = f;
}

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
	uint8_t mask = ioport_in(PIC1_DATA_PORT); // Get current mask
	mask &= 0xFD;
	ioport_out(PIC1_DATA_PORT, mask);
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
		if (g_keyboard_function == 0)
			return ;
		g_keyboard_function(keycode);
	}
}

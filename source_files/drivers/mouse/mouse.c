#include <IDT.h>
#include <mouse.h>
#include <vga_tty.h>
#include <stdbool.h>

// ASM function that gets called before the real handler
extern void	recieve_mouse_interrupt();

void (*g_mouse_function)(int16_t, int16_t, bool, bool) = 0;

void set_mouse_function(void (*f)(int16_t, in16_t, bool, bool))
{
	g_mouse_function = f;
}

/*

void mouse_handler()
{
}*/

static void wait_for_mouse(uint8_t input_type)
{
	uint32_t	time_out=100000;
	if(input_type==0)
	{
		while(--time_out) //Data
			if((ioport_in(0x64) & 1)==1)
				return;
	}
	else
	{
		while(--time_out) //Signal
			if((ioport_in(0x64) & 2)==0)
				return;
	}
	return;
}

static void write_to_mouse(uint8_t a_write) //unsigned char
{
	wait_for_mouse(1);		// Wait to be able tosend commands

	ioport_out(0x64, 0xD4);	// Tell the PS/2 that we want to talk to mouse
	wait_for_mouse(1);		// Wait for message-received OK

	// Tell our boy the good news.
	ioport_out(0x60, a_write);
}

// Get ACK(nowledgement) from mouse. Use this for 0xFA!
static uint8_t read_from_mouse()
{
	wait_for_mouse(0);
	return ioport_in(0x60);
}


static inline void setup_mouse_for_communication()
{
	// wait_for_mouse is ussed to wait until it is ready to receive commands
	// read_from_mouse is for the 0xFA ACKnowledgements.

	wait_for_mouse(1);
	ioport_out(0x64, 0xA8); // Set AUX to send IRQ12
	
	wait_for_mouse(1);
	ioport_out(0x64, 0x20); // Get compaq byte
	
	wait_for_mouse(0);
	uint8_t compaq_byte = (ioport_in(0x60) | 2); // Turn on bit 1
	
	wait_for_mouse(1);
	ioport_out(0x64, 0x60);	// Tell PS/2 we are about to set the config
	
	wait_for_mouse(1);
	ioport_out(0x60, compaq_byte); // Send our new compaq config

	write_to_mouse(0xF6);	// Tell mouse to use default settings
	read_from_mouse();
	
	write_to_mouse(0xF4);	// Enable packets
	read_from_mouse();
}


void init_mouse()
{
	setup_mouse_for_communication();

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
	//mask &= 0xBF;
	mask &= 0x01;
	ioport_out(PIC1_DATA_PORT, mask);
}

// This gets called on mouse interrupts
void handle_mouse_interrupt()
{
	static uint8_t	mouse_cycle=0;	// How many bytes are received so far
	static uint8_t	mouse_byte1;
	//static int8_t	mouse_byte[3];	// Mouse sends us a 3 bytes in it
					// This happens in separate IRQs
	static uint8_t	u_mouse_x = 0;	// Relative change in mouse_x
	static uint8_t	u_mouse_y = 0;	// Relative change in mouse_y
	static uint8_t	mouse_x_sign = 0;
	static uint8_t	mouse_y_sign = 0;

	switch(mouse_cycle)
	{
		case 0:
			mouse_byte1 = ioport_in(0x60);
			mouse_x_sign = (mouse_byte1 & 0b00010000) != 0;
			mouse_y_sign = (mouse_byte1 & 0b00100000) != 0;
			break;
		case 1:
			u_mouse_x = ioport_in(0x60);
			break;
		case 2:
			u_mouse_y = ioport_in(0x60);
		break;
	}
	mouse_cycle = (mouse_cycle + 1) % 3; // Limit range to 0-2

	if (mouse_cycle != 0) // We still have more packets to parse!
		return ;

	// Done reading the whole mouse packet, reset both PICs.
	ioport_out(PIC1_CMD_PORT, 0x20);
	ioport_out(PIC2_CMD_PORT, 0x20);

	if (g_mouse_function == 0)
		return ;
	bool	l_click = (mouse_byte1 & 0b00000001) != 0;
	bool	r_click = (mouse_byte1 & 0b00000010) != 0;

	int16_t	mouse_x = (u_mouse_x | (mouse_x_sign << 15));
	int16_t	mouse_y = (u_mouse_y | (mouse_y_sign << 15));

	terminal_putchar(mouse_x + '5');
	terminal_putchar(' ');
	terminal_putchar(mouse_y + '5');
	terminal_putchar('\n');
	g_mouse_function(mouse_x, mouse_y, l_click, r_click);
}

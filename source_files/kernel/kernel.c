/*
 * This file of course contains the IDT.
 * 
 * But it also hooks up interrupt drivers to it, such as the keyboard driver.
*/


#include "../libk/include/vga_tty.h"
#include <stdint.h>

/* The first 8 bytes of te GDT is the null segment
 * so the offset of the code segment is 8 bytes */
#define	KERNEL_SEGMENT_OFFSET	0x8

#define	IDT_SIZE		256
#define	IDT_INTERRUPT_GATE	0x8e

#define	PIC1_CMD_PORT		0x20
#define	PIC1_DATA_PORT		0x21
#define	PIC2_CMD_PORT		0xA0
#define	PIC2_DATA_PORT		0xA1

#define	KEYBOARD_DATA_PORT	0x60
#define	KEYBOARD_STATUS_PORT	0x64

// Pointer to the IDT. Similar to how there is a ptr to the GDT
struct	IDT_ptr
{
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed));
// packed attribute removes any padding the compiler might add

// One entry in the IDT. Again very similar to the GDT entries.
struct	IDT_entry
{
	uint16_t	offset_lowerbits;
	uint16_t	selector;
	uint8_t		zero;
	uint8_t		type_attribute;
	uint16_t	offset_upperbits;
} __attribute__((packed));

struct IDT_entry g_IDT[IDT_SIZE]; // The IDT itself.

extern void	keyboard_handler();
extern int8_t	ioport_in(uint16_t port);
extern void	ioport_out(uint16_t port, uint8_t data);
extern void	load_IDT(struct IDT_ptr *idt_address);
extern void	enable_interrupts();
extern void	disable_interrupts();
				  
void init_IDT()
{
	// This is the memory address of the function
	uint32_t	offset = (uint32_t)keyboard_handler;

	// Interrupts 0-32 are reserved for the CPU. 33 is the first free one.
	g_IDT[33].offset_lowerbits = offset & 0x0000FFFF; // Lower half
	g_IDT[33].selector = KERNEL_SEGMENT_OFFSET;
	g_IDT[33].zero = 0;
	g_IDT[33].type_attribute = IDT_INTERRUPT_GATE;
	g_IDT[33].offset_upperbits = (offset & 0xFFFF0000) >> 16; // Higher half

	// ICW1 ( init/reset PIC )
	ioport_out(PIC1_CMD_PORT, 0x11);
	ioport_out(PIC2_CMD_PORT, 0x11);

	// ICW2 ( vector offset aka. where is the interrupt request [IRQ] )
	ioport_out(PIC1_DATA_PORT, 0x20);
	ioport_out(PIC2_DATA_PORT, 0x28);

	// ICW3 ( cascade settings. We're not touching this for now.  )
	ioport_out(PIC1_DATA_PORT, 0x0);
	ioport_out(PIC2_DATA_PORT, 0x0);

	// ICW4 ( environment info. not 100% sure what it *really* does. )
	ioport_out(PIC1_DATA_PORT, 0x1);
	ioport_out(PIC2_DATA_PORT, 0x1);

	// Input masking. This is a bitmap.
	// If the interrupt is masked, it will simply be ignored.
	// We likely have unused IRQs so we want to mask everything by default.
	ioport_out(PIC1_DATA_PORT, 0xFF);
	ioport_out(PIC2_DATA_PORT, 0xFF);

	struct IDT_ptr idt_ptr;
	// Size is always - 1 with GDT and IDT. Don't remember why, but is.
	idt_ptr.limit = (sizeof(struct IDT_entry)) * IDT_SIZE - 1;
	idt_ptr.base = (uint32_t) &g_IDT;
	load_IDT(&idt_ptr);
}

void init_keyboard()
{
	/*
	* Unmask the keyboard IRQ, which is 1
	* 0xFD = 1111 1101
	* From right to left the ports are 0-7.
	* So we are setting port 1 as 0, aka not masked.
	*/
	ioport_out(PIC1_DATA_PORT, 0xFD);
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













/*
 *
 * This is the main kernel file.
 * It contains the primary abstract logic.
 *
 * External functions are heavily used. Mostly functions from libk.
 *
*/

//#include "../libk/include/vga_tty.h"


// Each row is 80 wide even if it doesn't look like it
void print_bootup_message()
{
	// Store original color so we can change back to it before returning
	uint8_t original_color = get_color();

	// Color of the first half
	uint8_t color = vga_block_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
	terminal_setcolor(color);

	unsigned char *h1 = "\
00000000000000000000000000000000000000000000000000000000000000000000000000000000\
1                                                                              1\
2      oooooooooo.     .oooooo.   ooooooooo.     .oooooo.    .oooooo..o        2\
3      `888'   `Y8b   d8P'  `Y8b  `888   `Y88.  d8P'  `Y8b  d8P'    `Y8        3\
4       888      888 888      888  888   .d88' 888      888 Y88bo.             4\
5       888      888 888      888  888ooo88P'  888      888  `\"Y8888o.         5\
6       888      888 888      888  888`88b.    888      888      `\"Y88b        6";

	// Print first half
	terminal_putstring(h1);
	// Change color for second half
	color = vga_block_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_RED);
	terminal_setcolor(color);

	unsigned char *h2 = "\
7       888     d88' `88b    d88'  888  `88b.  `88b    d88' oo     .d8P        7\
8      o888bood8P'    `Y8bood8P'  o888o  o888o  `Y8bood8P'  8\"\"88888P'         8\
9                                                                              9\
10101010101010101010101010101010101010101010101010101010101010101010101010101010";
	terminal_putstring(h2);

	// Subtitle color
	color = vga_block_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
	terminal_setcolor(color);

	unsigned char *subtitle= "\
                           - Welcome to DOROS v 0.1 -                           ";
	terminal_putstring(subtitle);

	// Switch color back to the original color
	terminal_setcolor(original_color);
}

void main()
{
	terminal_init();
	print_bootup_message();
	init_IDT();
	init_keyboard();
	enable_interrupts(); // Re-enable interrupts

	while (1)
	{}
}

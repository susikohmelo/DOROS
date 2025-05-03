#include "IDT.h"
#include "../../drivers/include/vga_tty.h"

#define ICW1_INIT	0x10 // For init in cascade mode
#define ICW1_ICW4	0x01 // Tells the pic we have ICW4

// Please check the IDT.h file for more info about the IDT struct(s)

struct IDT_entry g_IDT[IDT_SIZE]; // The IDT itself.

// Return ptr to the IDT
struct IDT_entry *get_IDT()
{
	return (g_IDT);
}

void init_IDT()
{
	// ICW1 ( init/reset PIC )
	ioport_out(PIC1_CMD_PORT, ICW1_INIT | ICW1_ICW4);
	ioport_out(PIC2_CMD_PORT, ICW1_INIT | ICW1_ICW4);

	// ICW2 ( vector offset aka. where is the interrupt request [IRQ] )
	ioport_out(PIC1_DATA_PORT, 0x20);
	ioport_out(PIC2_DATA_PORT, 0x28);

	// ICW3 ( cascade settings. Tell PIC1 that there is a slave at IRQ2  )
	ioport_out(PIC1_DATA_PORT, 4); // bits 0000 0100
	ioport_out(PIC2_DATA_PORT, 2); // bits 0000 0010

	// ICW4 ( environment info. not 100% sure what it *really* does. )
	ioport_out(PIC1_DATA_PORT, 0x01);
	ioport_out(PIC2_DATA_PORT, 0x01);

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

#pragma once

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

// General IDT functions
extern int8_t	ioport_in(uint16_t port);
extern void	ioport_out(uint16_t port, uint8_t data);
extern void	load_IDT(struct IDT_ptr *idt_address);

// Return ptr to the IDT
struct IDT_entry *get_IDT();

void init_IDT();

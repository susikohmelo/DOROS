#pragma once
// This is just a small header for the kernel to include and get access
// to the interrupt_utils ASM functions

#include "IDT.h"

extern int8_t	ioport_in(uint16_t port);
extern void	ioport_out(uint16_t port, uint8_t data);
extern void	load_IDT(struct IDT_ptr *idt_address);
extern void	enable_interrupts();
extern void	disable_interrupts();

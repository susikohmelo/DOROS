#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <vga_tty.h>
#include <heap.h>

void	k_terminal_putnbr(int32_t n);
void	k_terminal_putbits(uint8_t n);
int32_t k_atoi(const uint8_t *nptr);
void	k_memcpy(void *s, void *d, uint32_t len);

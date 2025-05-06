#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <vga_tty.h>
#include <heap.h>

void	k_terminal_putnbr(int32_t n);
int32_t k_atoi(const uint8_t *nptr);

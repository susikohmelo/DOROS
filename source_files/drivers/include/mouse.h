#pragma once

#include <stdbool.h>

#define	MOUSE_DATA_PORT		0x60
#define	MOUSE_STATUS_PORT	0x64

void init_mouse();
void handle_mouse_interrupt();

// The function that gets called on keypress.
// This way we don't have to do any polling.
void set_mouse_function(void (*f)(int16_t, int16_t, bool, bool));

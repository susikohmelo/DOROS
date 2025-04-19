#pragma once
// Short file indeed
// This is just used by the kernel to get a hold of these functions

void init_keyboard();
void handle_keyboard_interrupt();

// The function that gets called on keypress.
// This way we don't have to do any polling.
void set_keyboard_function(void (*f)(uint8_t));

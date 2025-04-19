#pragma once

void init_keyboard();
void handle_keyboard_interrupt();

// The function that gets called on keypress.
// This way we don't have to do any polling.
void set_keyboard_function(void (*f)(uint8_t));

enum special_keys
{
	KEY_ESCAPE		= -2,
	KEY_BACKSPACE		= -3,
	KEY_TAB			= -4,
	KEY_ENTER		= -5,
	KEY_LEFT_SHIFT		= -6,
	KEY_LEFT_CTRL		= -7,
	KEY_LEFT_ALT		= -8,
	KEY_RIGHT_SHIFT		= -9,
	KEY_F1			= -10,
	KEY_F2			= -11,
	KEY_F3			= -12,
	KEY_F4			= -13,
	KEY_F5			= -14,
	KEY_F6			= -15,
	KEY_F7			= -16,
	KEY_F8			= -17,
	KEY_F9			= -18,
	KEY_F10			= -19,
};


// -1 are characters that are not handled.
// Arrow keys are special and not handled yet.
static int8_t keycode_map[128] = {
	-1, KEY_ESCAPE,
	'1', '2', '3', '4', '5','6', '7', '8', '9', '0', '-', '=',
	KEY_BACKSPACE, KEY_TAB,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	KEY_ENTER, KEY_LEFT_CTRL,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' ,';', -1, '\'',
	KEY_LEFT_SHIFT,
	-1, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	KEY_RIGHT_SHIFT,
	'*', KEY_LEFT_ALT, ' ', -1,
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
	KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
	-1,
	-1,
	'7', '8', '9', '-', '4', '5', '6', -1, '1', '2', '3', '0', '.',
	-1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1

};

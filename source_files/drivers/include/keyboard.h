#pragma once

#define	KEYBOARD_DATA_PORT	0x60
#define	KEYBOARD_STATUS_PORT	0x64

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
	KEY_LEFT_SHIFT_RELEASE	= -7,
	KEY_LEFT_CTRL		= -8,
	KEY_LEFT_ALT		= -9,
	KEY_RIGHT_SHIFT		= -10,
	KEY_F1			= -11,
	KEY_F2			= -12,
	KEY_F3			= -13,
	KEY_F4			= -14,
	KEY_F5			= -15,
	KEY_F6			= -16,
	KEY_F7			= -17,
	KEY_F8			= -18,
	KEY_F9			= -19,
	KEY_F10			= -20,
	KEY_ARROW		= -21,
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

// This C file is directly included inside picoshell, so don't worry about
// any headers missing. It's intended to be like so.

static void cmd_ls(uint8_t *args)
{
	terminal_putstring("32-bit mode file system is not implemented yet\n");
}
static void cmd_clear(uint8_t *args)
{
	terminal_clear_screen();
}
static void cmd_help(uint8_t *args)
{
	// I'm sorry it looks funny, but this is the easiest way to format
	terminal_putstring("\
Format: command arg1 arg2 ...\n\n\
Commands:\n\
- help\n\
- clear\n\
- ls\n");
}

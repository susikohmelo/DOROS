#include "../../kernel/interrupts/IDT.h"
#include "../include/mouse.h"
#include "../include/vga_tty.h"

// ASM function that gets called before the real handler
extern void	recieve_mouse_interrupt();

void (*g_mouse_function)(uint32_t) = 0;

void set_mouse_function(void (*f)(uint32_t))
{
	g_mouse_function = f;
}

uint8_t	mouse_cycle=0;     //unsigned char
int8_t	mouse_byte[3];    //signed char
int8_t	mouse_x=0;         //signed char
int8_t	mouse_y=0;         //signed char

void mouse_handler()
{
  switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0]=ioport_in(0x60);
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1]=ioport_in(0x60);
      mouse_cycle++;
      break;
    case 2:
      mouse_byte[2]=ioport_in(0x60);
      mouse_x=mouse_byte[1];
      mouse_y=mouse_byte[2];
      mouse_cycle=0;
      break;
  }
}






inline void mouse_wait(uint8_t a_type) //unsigned char
{
	 int32_t _time_out=100000; //unsigned int
	  if(a_type==0)
	  {
	    while(_time_out--) //Data
	    {
	      if((ioport_in(0x64) & 1)==1)
	      {
	        return;
	      }
	    }
	    return;
	  }
	  else
	  {
	    while(_time_out--) //Signal
	    {
	      if((ioport_in(0x64) & 2)==0)
	      {
	        return;
	      }
	    }
	    return;
	  }
	}

inline void mouse_write(uint8_t a_write) //unsigned char
	{
	  //Wait to be able to send a command
	  mouse_wait(1);
	  //Tell the mouse we are sending a command
	  ioport_out(0x64, 0xD4);
	  //Wait for the final part
	  mouse_wait(1);
	  //Finally write
	  ioport_out(0x60, a_write);
}

uint8_t mouse_read()
{
  //Get's response from mouse
  mouse_wait(0);
  return ioport_in(0x60);
}


void install_mouse()
{
   mouse_wait(1);
   ioport_out(0x64, 0xA8);

   mouse_wait(1);
	ioport_out(0x64, 0x20);

	uint8_t status_byte;
	mouse_wait(0);
	status_byte = (ioport_in(0x60) | 2);


	mouse_wait(1);
	ioport_out(0x64, 0x60);


	mouse_wait(1);
	ioport_out(0x60, status_byte);


	mouse_write(0xF6);
	mouse_read();

	mouse_write(0xF4);
	mouse_read();

}








void init_mouse()
{
	install_mouse();

	// The IDT is just an array of entries.
	struct IDT_entry *IDT = get_IDT();

	// This is the memory address of the function
	uint32_t	offset = (uint32_t)recieve_mouse_interrupt;

	// Interrupts 0-32 are reserved for the CPU. 32 + 12 is our interrupt
	IDT[32 + 12].offset_lowerbits = offset & 0x0000FFFF; // Lower half
	IDT[32 + 12].selector = KERNEL_SEGMENT_OFFSET;
	IDT[32 + 12].zero = 0;
	IDT[32 + 12].type_attribute = IDT_INTERRUPT_GATE;
	IDT[32 + 12].offset_upperbits = (offset & 0xFFFF0000) >> 16; // Higher half

	/*
	* Unmask the mouse IRQ, which is 12 (starts from 8)
	* 0xEF = 1110 1111
	* From right to left the ports are 0-7.
	* So we are setting port 1 as 0, aka not masked.
	*/
	uint8_t mask = ioport_in(PIC2_DATA_PORT); // Get current mask
	//mask &= 0xEF;
	mask &= 0x00;
	ioport_out(PIC2_DATA_PORT, mask);

	// We also need to open up IRQ 2 on PIC1
	mask = ioport_in(PIC1_DATA_PORT); // Get current mask
	//mask &= 0xBF;
	mask &= 0x01;
	ioport_out(PIC1_DATA_PORT, mask);
}

// This gets called on mouse interrupts
void handle_mouse_interrupt()
{
	terminal_putstring("WHAH");
	// Reset command - yank that interrupt off the queue
	ioport_in(0x60);
	ioport_out(PIC2_CMD_PORT, 0x20);
	ioport_out(PIC1_CMD_PORT, 0x20);
	return;

	// Yoink the status
	uint8_t status = ioport_in(MOUSE_STATUS_PORT);

	if (status & 0x1) // If there is a new input to be had
	{
		uint32_t x = ioport_in(MOUSE_DATA_PORT);
		if (g_mouse_function == 0)
			return ;
		g_mouse_function(x);
	}
}

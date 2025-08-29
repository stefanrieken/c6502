#include <stdlib.h>

#include "display.h"

static unsigned char memory[65536];

unsigned char mem_get(unsigned short address)
{
	if (address == 0x00FE)
		return rand() % 256;

	return memory[address];
}

void mem_set(unsigned short address, unsigned char value)
{
//	printf("%x = %x\n", address, value);

	memory[address] = value;

	if (address >= 0x0200 && address < 0x0600)
		display_redraw();
}

#include <stdio.h>

#include "memory.h"

void load(char * filename)
{
	FILE * file = fopen(filename, "rb");
	
	int ch = fgetc(file);
	unsigned short current = 0x0600;

	while(ch >= 0)
	{
		mem_set(current++, (unsigned char) ch);
		ch = fgetc(file);
	}

	fclose(file);

	// The ROM reset vector
	mem_set(0xFFFC, (short) 0x00);
	mem_set(0xFFFD, (short) 0x06);

}

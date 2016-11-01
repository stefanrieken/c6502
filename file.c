#include <stdio.h>

#include "memory.h"

void load(char * filename)
{
	FILE * file = fopen(filename, "r");
	
	int ch = fgetc(file);
	unsigned short current = 0x200;
	//unsigned short current = 0x0600;

	while(ch >= 0)
	{
		mem_set(current++, (unsigned char) ch);
		ch = fgetc(file);
	}

	fclose(file);
}

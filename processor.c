#include <stdio.h>
#include <stdbool.h>

#include "memory.h"
#include "decode.h"

unsigned short pc;
unsigned char sp;
unsigned char a;
unsigned char x;
unsigned char y;

void (* jump00modeless1[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

unsigned short word(unsigned char lo, unsigned char hi)
{
	return 256 * hi + lo;
}

unsigned char hi (unsigned short word) {
	return word >> 8;
}

unsigned char lo (unsigned short word) {
	return word & 0xFF;
}


void p_reset()
{
	pc = word(mem_get(0xFFFC), mem_get(0xFFFD));
}

bool p_run()
{
	printf("pc=%x\n", pc);
	unsigned char opcode = mem_get(pc++);
	printf("opcode=%x\n", opcode);

	if (opcode == 0x60 && sp == 0xFF)
	{
		printf("Exiting on final RTS\n");
		return false;
	}

	// using the description on http://www.llx.com/~nparker/a2/opcodes.html,
	// split opcode in aaabbbcc
//	unsigned char aaa = opcode >> 5;
//	unsigned char bbb = (opcode & 0b11100) >> 2;
//	unsigned char cc = opcode & 0b11;

//	if (cc < 0b11) // 0b11 does not exist as an option
	if ((opcode & 0b11) < 0b11)
		decode_and_run(opcode);

	if (opcode == 0) printf("Exiting on BRK\n");
	return opcode != 0;
}

// TODO turn this into a jump table as well
unsigned short resolveAddress(unsigned char bbb) {
	if (bbb == 0b000) { // (zero page,X)
		// Turns out that in the examples this mode is mostly used
		// with x=0, which hides the subtle difference with (zp),y
		unsigned short zeroPageAddress = word(mem_get(pc++), 0) + x;
		return word(mem_get(zeroPageAddress), mem_get(zeroPageAddress+1));
	} else if (bbb == 0b001) { // zero page
		return word(mem_get(pc++), 0);
	} else if (bbb == 0b010) {
		return mem_get(pc++);
	} else if (bbb == 0b011) { // absolute
		int lo = mem_get(pc++); // have to do this separately because evaluation order of the below is not guaranteed
		return word(lo, mem_get(pc++));
	} else if (bbb == 0b100) { // (zero page), y
		int zeroPageAddress = word(mem_get(pc++), 0);
		// looking for the word stored at the two consecutive zero-page locations
		printf("Y: %x\n", y);
		return word(mem_get(zeroPageAddress), mem_get(zeroPageAddress+1)) + y;
	} else if (bbb == 0b110) { // absolute, y
		int lo = mem_get(pc++);
		return word(lo, mem_get(pc++)) + y;
	} else if (bbb == 0b111) { // absolute, x
		int lo = mem_get(pc++);
		int address=word(lo, mem_get(pc++));
		return address + x;
	} else if (bbb == 0b1000) { // NOT a true bbb value, but used to substitute zp,X with zp,Y on STX / LDX
		unsigned short zeroPageAddress = word(mem_get(pc++), 0) + y;
		return word(mem_get(zeroPageAddress), mem_get(zeroPageAddress+1));
	} else if (bbb == 0b1001) { // NOT a true bbb value, but to communicate the 65c02 (zp) addressing mode
		unsigned short zeroPageAddress = word(mem_get(pc++), 0);
		return word(mem_get(zeroPageAddress), mem_get(zeroPageAddress+1));
	}
	
	return 0; // shouldn't get here
}

unsigned char resolveOperand(unsigned char bbb, unsigned char cc) {
	if (cc == 0b01) {
		if (bbb == 0b010) return mem_get(pc++); // immediate
		else return mem_get(resolveAddress(bbb));
	} else { // cc == 00, cc == 10
		if (bbb == 0b000) return mem_get(pc++); // immmediate
		else return mem_get(resolveAddress(bbb));
	}
}


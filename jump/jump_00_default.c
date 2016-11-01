#include <stdio.h>

#include "../memory.h"
#include "../processor.h"
#include "../alu.h"


void brk (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// if bbb == 000
	// TODO jump to interrupt routine
}


void bitOrJsrAbs (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb == 0b000) { // JSR abs
		unsigned short returnAddress = pc + 2;
		alu_push(hi(returnAddress));
		alu_push(lo(returnAddress));
		pc = resolveAddress((unsigned char) 0b011); // explicitly absolute, because instruction is irregular
	} else { // BIT
		alu_bit(a, resolveOperand(bbb,cc));
	}

}

void jmpAbsOrRti (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb == 0b000) { // RTI
		sr = alu_pull();
		pc = alu_pullWord();
	} else { // JMP (abs)
		unsigned short address = resolveAddress(0b011);
		printf("JMP (abs) address: %x\n", address);
		pc = address; // explicitly absolute, because instruction is irregular
	}

}

void jmpIndOrRts (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb == 0b000) { // RTS
		pc = alu_pullWord();
	} else { // JMP (ind)
		unsigned short address = resolveAddress(bbb);
		pc = word(mem_get(address), mem_get(address + 1));
	}

}

void sty (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	mem_set(resolveAddress(bbb), y);
}

void ldy (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	y = mem_get(resolveOperand(bbb, cc));
}


void cpy (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	alu_check(y - resolveOperand(bbb, cc));
}

void cpx (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	alu_check(x - resolveOperand(bbb, cc));
}

void (* jump_00_default[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
	brk, bitOrJsrAbs, jmpAbsOrRti, jmpIndOrRts,
	sty, ldy, cpy, cpx
};


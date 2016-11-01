#include <stdio.h>

#include "../memory.h"
#include "../processor.h"
#include "../alu.h"

#include "jump_00_modeless.h"
#include "jump_00_default.h"

/* Jump based on aaa for an assorted group of mostly 'modeless' instructions */
static void jmpMod(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	jump_00_modeless[aaa](aaa, bbb, cc);
}

/* Branch instructions */
static void branch(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// xx gives the status bit to test:
	// 00=SIGN,01=OVFL,10=CARY,11=ZERO
	// y gives the value to compare to
	unsigned char xx = aaa >> 1;
	unsigned char y = aaa & 0b1;
	
	// all branch instructions have two's complement relative addressing.
	char relative = mem_get(pc++);

	char * mnemonic; // TODO implement elsewhere for debugging purposes

	bool test = false;
	if (xx == 0b00) { test = (sr & SIGN) != 0; mnemonic = y == 1 ? "BMI" : "BPL"; };
	if (xx == 0b01) { test = (sr & OVFL) != 0; mnemonic = y == 1 ? "BVS" : "BVC"; };
	if (xx == 0b10) { test = (sr & CARY) != 0; mnemonic = y == 1 ? "BCS" : "BCC"; }; 
	if (xx == 0b11) { test = (sr & ZERO) != 0; mnemonic = y == 1 ? "BEQ" : "BNE"; };

	if ((test && y == 1) || (!test && y == 0))
	{
		printf("Branching relative: %d\n", relative);
		pc += relative;
	}

}

/* Set or clear status register bits */
static void setClr(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (aaa == 0b100) { // exception: TYA
		a = alu_check(y);
		return;
	}

	// xx gives the status bit to clear or set:
	// 00=CARY,01=INTR,10=SIGN,11=DECI
	// y indicates set or clear
	unsigned short xx = aaa >> 1;
	unsigned short y = aaa & 0b1;
	short which = 0;

	char * mnemonic; // TODO implement elsewhere for debugging purposes

	if (xx == 0b00) { which = CARY; mnemonic = y == 1 ? "SEC" : "CLC"; };
	if (xx == 0b01) { which = INTR; mnemonic = y == 1 ? "SEI" : "CLI"; };
	if (xx == 0b10) { which = OVFL; mnemonic = y == 1 ? "SEV" : "CLV"; }; 
	if (xx == 0b11) { which = DECI; mnemonic = y == 1 ? "SED" : "CLD"; };

	if (y == 1) sr |= which;
	else sr &= (0xFF - which);

}

/* This is the default action: jump further, based on aaa */
void jmpAaa(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	jump_00_default[aaa](aaa, bbb, cc);
}


void (* jump_00[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
        jmpAaa, jmpAaa, jmpMod, jmpAaa,
        branch, jmpAaa, setClr, jmpAaa
};


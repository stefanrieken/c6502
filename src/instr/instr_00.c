#include <stdio.h>

#include "../memory.h"
#include "../processor.h"
#include "../alu.h"


void brk (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// if bbb == 000
	// TODO jump to interrupt routine
}


void jsr(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    unsigned short returnAddress = pc + 2;
    alu_push(hi(returnAddress));
    alu_push(lo(returnAddress));
    pc = resolveAddress((unsigned char) 0b011); // explicitly absolute, because instruction is irregular
}

void jmp(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    unsigned short address = resolveAddress(bbb);
//    printf("JMP (abs) address: %x\n", address);
    pc = address; // explicitly absolute, because instruction is irregular
}

// Jump indirect
void jp2 (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    unsigned short address = resolveAddress(bbb);
    pc = word(mem_get(address), mem_get(address + 1));
}

void rti(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    sr = alu_pull();
    pc = alu_pullWord();
}

void rts(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    pc = alu_pullWord();
}

void bit(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    alu_bit(a, resolveOperand(bbb,cc));
}

void txs(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    sp = alu_check(x);
}

void tsx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    x = alu_check(sp);
}

void tya(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    a = alu_check(y);
}

void sty (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    mem_set(resolveAddress(bbb), y);
}

void ldy (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    y = alu_check(resolveOperand(bbb, cc));
//    printf("Y: %x\n", y);
}


void cpy (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    alu_check(y - resolveOperand(bbb, cc));
}

void cpx (unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    alu_check(x - resolveOperand(bbb, cc));
}

void php(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	alu_push(sr);
}

void plp(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	sr = alu_pull();
}

void pha(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	alu_push(a);
}

void pla(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_pull();
}

void dey(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	y = alu_dec(y);
}

void tay(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	y = alu_check(a);
}

void iny(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	y = alu_inc(y);
}

void inx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	x = alu_inc(x);
}

/* Branch instructions */
void br_(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// xx gives the status bit to test:
	// 00=SIGN,01=OVFL,10=CARY,11=ZERO
	// y gives the value to compare to
	unsigned char xx = aaa >> 1;
	unsigned char y = aaa & 0b1;
	
	// all branch instructions have two's complement relative addressing.
	char relative = (char) mem_get(pc++); // cast to signed char

	char * mnemonic; // TODO implement elsewhere for debugging purposes

	bool test = false;
	if (xx == 0b00) { test = (sr & SIGN) != 0; mnemonic = y == 1 ? "BMI" : "BPL"; };
	if (xx == 0b01) { test = (sr & OVFL) != 0; mnemonic = y == 1 ? "BVS" : "BVC"; };
	if (xx == 0b10) { test = (sr & CARY) != 0; mnemonic = y == 1 ? "BCS" : "BCC"; }; 
	if (xx == 0b11) { test = (sr & ZERO) != 0; mnemonic = y == 1 ? "BEQ" : "BNE"; };

	if ((test && y == 1) || (!test && y == 0))
	{
//		printf("sr: %d\n", sr);
//		printf("Branching relative: %d\n", relative);
		pc += relative;
	}

}

/* Clear or set status register bits */
void c_s(unsigned char aaa, unsigned char bbb, unsigned char cc)
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

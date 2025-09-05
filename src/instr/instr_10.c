#include "../memory.h"
#include "../processor.h"
#include "../alu.h"

void asl(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb = 0b010) { a = alu_aslRol(a, false); return; } // accumulator mode
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_aslRol(mem_get(address), false));
}

void rol(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb = 0b010) { a = alu_aslRol(a, true); return; } // accumulator mode
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_aslRol(mem_get(address), true));
}

void lsr(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb = 0b010) { a = alu_lsrRor(a, false); return; } // accumulator mode
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_lsrRor(mem_get(address), false));
}

void ror(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb = 0b010) { a = alu_lsrRor(a, true); return; } // accumulator mode
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_lsrRor(mem_get(address), true));
}

void stx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// As STX indexed by X doesn't make much sense, use Y instead of X
	if (bbb == 0b101) bbb = 0b1000; // Change zp,X to zp,Y addressing
	mem_set(resolveAddress(bbb), x);
}

void ldx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// As LDX indexed by X doesn't make much sense, use Y instead of X
	if (bbb == 0b101) bbb = 0b1000; // Change zp,X to zp,Y addressing
	if (bbb == 0b111) bbb = 0b110;  // change abs,X to abs,Y addressing
	x = alu_check(resolveOperand(bbb, cc));
}

void dec(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_dec(mem_get(address)));
}

void inc(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_inc(mem_get(address)));
}

void (* jump_10[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
	asl, rol, lsr, ror,
	stx, ldx, dec, inc
};

void txa(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_check(x);
}

void tax(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	x = alu_check(a);
}

void dex(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	x = alu_dec(x);
}

void nop(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// nope
}


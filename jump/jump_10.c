#include "../memory.h"
#include "../processor.h"
#include "../alu.h"

void asl(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_aslRol(mem_get(address), false));
}

void rol(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_aslRol(mem_get(address), true));
}

void lsr(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_lsrRor(mem_get(address), false));
}

void ror(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	unsigned short address = resolveAddress(bbb);
	mem_set(address, alu_lsrRor(mem_get(address), true));
}

void stx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	mem_set(resolveAddress(bbb), x);
}

void ldx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
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


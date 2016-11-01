#include "../processor.h"
#include "../alu.h"

// calls in the 'accumulator' semi-addressing mode, plus other modeless calls

static void asl(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_aslRol(a, false);
}

static void rol(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_aslRol(a, true);
}

static void lsr(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_lsrRor(a, false);
}

static void ror(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_lsrRor(a, true);
}

static void txa(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_check(x);
}

static void tax(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	x = alu_check(a);
}

static void dex(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	x = alu_dec(x);
}

static void nop(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	// nope
}

void (* jump_10_accumulator[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
	asl, rol, lsr, ror,
	txa, tax, dex, nop
};


#include "../memory.h"
#include "../processor.h"
#include "../alu.h"

static void ora(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_or(a, resolveOperand(bbb, cc));
}

static void and(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_and(a, resolveOperand(bbb, cc));
}

static void eor(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_xor(a, resolveOperand(bbb, cc));
}

static void adc(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_adc(a, resolveOperand(bbb, cc));
}

static void sta(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	mem_set(resolveAddress(bbb), a);
}

static void lda(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_check(resolveOperand(bbb, cc));
}

static void cmp(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	alu_cmp(a, resolveOperand(bbb, cc));
}

static void sbc(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_sbc(a, resolveOperand(bbb, cc));
}

void (* jump_01[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
	ora, and, eor, adc,
	sta, lda, cmp, sbc
};


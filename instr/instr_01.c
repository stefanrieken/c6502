#include "../memory.h"
#include "../processor.h"
#include "../alu.h"

void ora(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_or(a, resolveOperand(bbb, cc));
}

void and(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_and(a, resolveOperand(bbb, cc));
}

void eor(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_xor(a, resolveOperand(bbb, cc));
}

void adc(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_adc(a, resolveOperand(bbb, cc));
}

void sta(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	mem_set(resolveAddress(bbb), a);
}

void lda(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_check(resolveOperand(bbb, cc));
}

void cmp(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	alu_cmp(a, resolveOperand(bbb, cc));
}

void sbc(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	a = alu_sbc(a, resolveOperand(bbb, cc));
}


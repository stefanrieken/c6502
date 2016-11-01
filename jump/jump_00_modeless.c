#include "../processor.h"
#include "../alu.h"

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

void (* jump_00_modeless[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
	php, plp, pha, pla,
	dey, tay, iny, inx
};


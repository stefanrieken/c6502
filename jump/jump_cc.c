#include <stdlib.h>

#include "../processor.h"
#include "../alu.h"
#include "jump_00.h"
#include "jump_01.h"
#include "jump_10.h"
#include "jump_10_accumulator.h"

void jump00(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	jump_00[bbb](aaa, bbb, cc);
}

void jump01(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	jump_01[aaa](aaa, bbb, cc);
}

void jump10(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
	if (bbb == 0b010)
		jump_10_accumulator[aaa](aaa, bbb, cc);
	else if (bbb == 0b110)
	{
		if (aaa == 0b100) sp = alu_check(x);
		else if (aaa == 0b101) x = alu_check(sp);

	}
	else
		jump_10[aaa](aaa, bbb, cc);
}

void (* jump_cc[])(unsigned char aaa, unsigned char bbb, unsigned char cc) =
{
	jump00, jump01, jump10, NULL /* there is no run11 */
};


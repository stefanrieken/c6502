#include <stdbool.h>

#include "memory.h"
#include "processor.h"

unsigned char SIGN = 0b10000000;
unsigned char OVFL = 0b01000000;
unsigned char BREA = 0b00010000;
unsigned char DECI = 0b00001000;
unsigned char INTR = 0b00000100;
unsigned char ZERO = 0b00000010;
unsigned char CARY = 0b00000001;

unsigned char sr = 0;


void alu_sr(unsigned char bit, bool condition)
{
	if (condition) sr |= bit;
	else sr &= (0xFF - bit);
}

// test value with potential overflow for flags
// return value without overflow
unsigned char alu_check (short arg)
{
	alu_sr(SIGN, (arg & 0x80 != 0));
	alu_sr(ZERO, (arg & 0xFF == 0));

	return (unsigned char) arg & 0xFF;
}

unsigned char alu_or(unsigned char reg, unsigned char arg)
{
	return alu_check(reg | arg);
}

unsigned char alu_and(unsigned char reg, unsigned char arg)
{
	return alu_check(reg & arg);
}

unsigned char alu_xor(unsigned char reg, unsigned char arg)
{
	return alu_check(reg ^ arg);
}

unsigned char alu_adc(unsigned char reg, unsigned char arg)
{
	short result = reg + arg;
	if ((sr & CARY) != 0) result += 1;
	alu_sr(CARY, result > 255);
	result = alu_check(result);
	alu_sr(OVFL, (reg & 0x80) == 0 && result & 0x80 != 0);
	return result;
}


unsigned char alu_sbc(unsigned char reg, unsigned char arg)
{
	short result = reg - arg;
	if ((sr & CARY) != 0) result -= 1;
	alu_sr(CARY, result > 255);
	result = alu_check(result);
	alu_sr(OVFL, (reg & 0x80) == 0 && result & 0x80 != 0);
	return result;
}

unsigned char alu_inc(unsigned char reg)
{
	short result = reg+1;
	return alu_check(result & 0xFF);
}


unsigned char alu_dec(unsigned char reg)
{
	short result = reg-1;
	return alu_check(result & 0xFF);
}

void alu_cmp (unsigned char reg, unsigned short arg)
{
	short result = reg - arg;
	alu_check(result);
	alu_sr(CARY, result >255);
}

unsigned char alu_aslRol(unsigned char reg, bool rotate)
{
	bool oldCarry = sr & CARY != 0;
	short result = reg << 1;
	bool newCarry = result > 0xFF;
	alu_sr(CARY, newCarry);
	if(rotate && oldCarry) result &= 0x01;
	return result & 0xFF;
}


unsigned char alu_lsrRor(unsigned char reg, bool rotate)
{
	bool oldCarry = sr & CARY != 0;
	short result = reg >> 1;
	bool newCarry = reg & 0x01 != 0;
	alu_sr(CARY, newCarry);
	if(rotate && oldCarry) result &= 0x80;
	return result;
}

void alu_bit(unsigned char reg, unsigned char arg)
{
	alu_sr(ZERO, (reg & arg) == 0);
	alu_sr(OVFL, (reg & 0x06) != 0);
	alu_sr(SIGN, (reg & 0x07) != 0);
}

// Stack operations TODO wrap sp
void alu_push (unsigned char value)
{
	mem_set(word(sp--, 0x01), value);
}

unsigned char alu_pull()
{
	return alu_check(mem_get(word(sp++, 0x01)));
}

unsigned short alu_pullWord()
{
	unsigned char lo = alu_pull();
	unsigned char hi = alu_pull();
	return word(lo, hi);
}

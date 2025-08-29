#include "../memory.h"
#include "../processor.h"
#include "../alu.h"

// Instructions that are fully new with the 65c02.

void bra(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    char relative = (char) mem_get(pc++); // cast to signed char
    pc += relative;
}

void phy(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    alu_push(y);
}

void ply(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    y = alu_pull();
}

void phx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    alu_push(x);
}

void plx(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    x = alu_pull();
}

void tsb(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    unsigned short address = resolveAddress(bbb); // TSB has been fit to match address mode (either zp or abs)
    unsigned char mask = mem_get(address);
    alu_sr(ZERO, (a & mask) == 0);
    mem_set(resolveAddress(bbb), a & mask);
}

void trb(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    unsigned short address = resolveAddress(bbb); // decoder has made bbb regular for us (either zp or abs)
    unsigned char mask = mem_get(address);
    alu_sr(ZERO, (a & mask) == 0);
    mem_set(resolveAddress(bbb), a & ~mask);
}

void stz(unsigned char aaa, unsigned char bbb, unsigned char cc)
{
    mem_set(resolveAddress(bbb), 0);
}


#include <stdbool.h>

extern unsigned char SIGN;
extern unsigned char OVFL;
extern unsigned char BREA;
extern unsigned char DECI;
extern unsigned char INTR;
extern unsigned char ZERO;
extern unsigned char CARY;

extern unsigned char sr;


void alu_sr(unsigned char bit, bool condition);

// test value with potential overflow for flags
// return value without overflow
unsigned char alu_check (short arg);
unsigned char alu_or(unsigned char reg, unsigned char arg);
unsigned char alu_and(unsigned char reg, unsigned char arg);
unsigned char alu_xor(unsigned char reg, unsigned char arg);
unsigned char alu_adc(unsigned char reg, unsigned char arg);
unsigned char alu_sbc(unsigned char reg, unsigned char arg);
unsigned char alu_inc(unsigned char reg);
unsigned char alu_dec(unsigned char reg);
void alu_cmp (unsigned char reg, unsigned short arg);
unsigned char alu_aslRol(unsigned char reg, bool rotate);
unsigned char alu_lsrRor(unsigned char reg, bool rotate);
void alu_bit(unsigned char reg, unsigned char arg);
// Stack operations TODO wrap sp
void alu_push (unsigned char value);
unsigned char alu_pull();
unsigned short alu_pullWord();

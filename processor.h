#include <stdbool.h>

extern unsigned short pc;
extern unsigned char sp;
extern unsigned char a;
extern unsigned char x;
extern unsigned char y;

unsigned short word(unsigned char lo, unsigned char hi);
unsigned char hi (unsigned short word);

unsigned char lo (unsigned short word);

unsigned char resolveOperand(unsigned char bbb, unsigned char cc);
unsigned short resolveAddress(unsigned char bbb);

void p_reset();
bool p_run();

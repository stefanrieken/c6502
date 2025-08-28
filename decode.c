/*
All fully regular instructions could be handled by a statement such as:

        instruction[cc][aaa](resolveAddress(bbb));

Except for some 65c02 one-offs, all exceptions lie at cc = x0, bbb = xx0; so we
can track them in an 'overlay' registry, to be checked before the regular
registry.

Finally we leave resolving bbb to the functions, so that they can handle
addressing mode exceptions. In this sense it seems the most logical to treat
bbb = 000 as (zp, X) as the rule, and the #immediate variant as an exception.

        if (opcode == 0x89) instr = bit_imm; else ...                      // Test 65C02 one-offs: 1x BIT, 2x TSB (= regular), 2x TRB, 4x STZ
        else if (!(cc & bbb & 1)) { instr = irregular[cc>>1][bbb>>1][aaa]; // up to (but less than) 64 irregular instructions
            if(cc == 0b10 && bbb == 0b100) bbb = 0b1000; }                 // set new (zp) mode in 65c02
        if (instr == NULL) instr = regular[cc][aaa];                       // 24 regular instructions (in different addressing modes)
        instr(bbb);
*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "processor.h"
#include "alu.h"

#include "instr/instr.h"

Instruction irregular[2][4][8] = {
    // cc = 00
    {
        // bb = xx0
        {brk, jsr, rti, rts, bra, NULL, NULL, NULL}, // fall back to ldy, cpy, cpx
        {php, plp, pha, pla, dey, tay, iny, inx},
        {br_, br_, br_, br_, br_, br_, br_, br_},
        {c_s, c_s, c_s, c_s, tya, c_s, c_s, c_s}
    },
    // cc = 10
    {
        // bb = xx0
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, // All but ldx are invalid, even on 65c02 (fallbacks mostly end up being NOPs)
        {NULL, NULL, NULL, NULL, txa,  tax,  dex,  nop }, // Fall back to ora, and, eor, sbc
        {ora,  and,  eor,  adc,  sta,  lda,  cmp,  sbc },  // 65c02: as regular cc=01 but (zp)
        {inc,  dec,  phy,  ply,  txs,  tsx,  phx,  plx }
    }
};

Instruction regular[3][8] = {
    // cc = 00
    {tsb, bit, jmp, jp2, sty, ldy, cpy, cpx},
    // cc = 01
    {ora, and, eor, adc, sta, lda, cmp, sbc},
    // cc = 10
    {asl, rol, lsr, ror, stx, ldx, dec, inc}
};

void decode_and_run(uint8_t opcode)
{
    unsigned char aaa = opcode >> 5;
    unsigned char bbb = (opcode & 0b11100) >> 2;
    unsigned char cc = opcode & 0b11;

    Instruction instr = NULL;

#ifdef WDC65c02
    if (opcode == 0x89) instr = bit_imm;                    // TODO check all 65c02 one-offs: 1x BIT, 2x TSB (= regular), 2x TRB, 4x STZ
    //else ...
    else
#endif
    if (!((cc | bbb) & 1)) {                               // Irregular instructions are found in cc = x0 and bbb = x0
        instr = irregular[cc>>1][bbb>>1][aaa];             // up to (but less than) 64 irregular instructions
#ifdef WDC65c02
        if(cc == 0b10 && bbb == 0b100) bbb = 0b1001;       // set new (zp) mode in 65c02
#endif
    }
    if (instr == NULL) instr = regular[cc][aaa];                       // 24 regular instructions (in different addressing modes)
    if (instr == NULL) printf("Error: instr %x not defined\n");
    instr(aaa, bbb, cc);
}


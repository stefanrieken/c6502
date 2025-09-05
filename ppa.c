// The Parts Picker Assembler
//
// Constructs 6502 code from individual components
// on the principle that most instructions follow aaabbbcc,
// where cc is a group, aaa is an instruction and bbb is an
// addressing mode. 
//
// Where alternative encodings apply, we follow these instead.
// For this reasons the mnemonics for clear / set, branch,
// and push / pop instructions deviate from their normal form.

#include <stdio.h>
#include <stdint.h>

// Address modes

const uint8_t IMP = 0b000 << 2; // Implied
const uint8_t ZXI = 0b000 << 2; // Zero page X indirect: (z, X)
const uint8_t ZP  = 0b001 << 2; // Zero page
const uint8_t IMM = 0b010 << 2;
const uint8_t ACC = 0b010 << 2; // Note: same as IMM, so catch the difference!
const uint8_t ABS = 0b011 << 2;
const uint8_t ZIY = 0b100 << 2; // Zero page indirect, Y: (z),Y
const uint8_t ZX  = 0b101 << 2; // Zero page, X
const uint8_t ZY  = 0b101 << 2; // Zero page, Y; irregular alternative
const uint8_t STK = 0b110 << 2; // cc=10 'hack' / irregular alternative: 'stack' addressing mode
const uint8_t AY  = 0b110 << 2; // Absolute, Y
const uint8_t AX  = 0b111 << 2; // Absolute, X

// Regular instructions

const uint8_t TSB = (0b000 << 5) | 0b00; // 65c02 only
const uint8_t BIT = (0b001 << 5) | 0b00;
const uint8_t JMP = (0b010 << 5) | 0b00;
const uint8_t JPI = (0b011 << 5) | 0b00;
const uint8_t STY = (0b100 << 5) | 0b00;
const uint8_t LDY = (0b101 << 5) | 0b00;
const uint8_t CPY = (0b110 << 5) | 0b00;
const uint8_t CPX = (0b111 << 5) | 0b00;

const uint8_t ORA = (0b000 << 5) | 0b01;
const uint8_t AND = (0b001 << 5) | 0b01;
const uint8_t EOR = (0b010 << 5) | 0b01;
const uint8_t ADC = (0b011 << 5) | 0b01;
const uint8_t STA = (0b100 << 5) | 0b01;
const uint8_t LDA = (0b101 << 5) | 0b01;
const uint8_t CMP = (0b110 << 5) | 0b01;
const uint8_t SBC = (0b111 << 5) | 0b01;

const uint8_t ASL = (0b000 << 5) | 0b10;
const uint8_t ROL = (0b001 << 5) | 0b10;
const uint8_t LSR = (0b010 << 5) | 0b10;
const uint8_t ROR = (0b011 << 5) | 0b10;
const uint8_t STX = (0b100 << 5) | 0b10;
const uint8_t LDX = (0b101 << 5) | 0b10;
const uint8_t DEC = (0b110 << 5) | 0b10;
const uint8_t INC = (0b111 << 5) | 0b10;

// Clear / set instructions
const uint8_t CLR = (0b0110 << 2) | 0b00;
const uint8_t SET = (0b1110 << 2) | 0b00;
const uint8_t CARY = (0b00 << 6);
const uint8_t INTR = (0b01 << 6);
const uint8_t OVFL = (0b10 << 6);
const uint8_t BCD  = (0b11 << 6);

// Branch instructions
const uint8_t BRC = (0b0100 << 2) | 0b00; // Branch if clear
const uint8_t BRS = (0b1100 << 2) | 0b00; // Branch if set
// Registers use different encoding than with clear / set
const uint8_t N = (0b00 << 6); // Negative flag
const uint8_t V = (0b01 << 6); // Overflow flag
const uint8_t C = (0b10 << 6); // Carry flag
const uint8_t Z = (0b11 << 6); // Zero flag

// Push / pull SR / Acc
const uint8_t PSH = (0b0010 << 2);
const uint8_t POP = (0b1010 << 2);
const uint8_t SR  = (0b00 << 6);
const uint8_t AR  = (0b01 << 6);

// The TXA, TSX family of functions can be synthesized as:
// TXA = STX | ACC; TSX = LDX | STACK; etc.
// TYA is still irregular (and TAY half); therefore
// it pays to just write these out:

const uint8_t TXA = STX | ACC | 0b10;
const uint8_t TAX = LDX | ACC | 0b10;
const uint8_t TYA = 0b10011000; // Actually fully irregular
const uint8_t TAY = LDY | ACC | 0b00; // Still quite irregular
const uint8_t TXS = STX | STK | 0b10;
const uint8_t TSX = LDX | STK | 0b10;

// That leaves us with the fully irregular instructions.
// As with the alternative encodings above, we can detect
// these mnemonics by their bbb placeholder not being 000.

// The first 4 instructions in cc=00 are an exception:
const uint8_t BRK = 0b00000000;
const uint8_t JSR = 0b00100000;
const uint8_t RTI = 0b01000000;
const uint8_t RTS = 0b11000000;

// But these are all detectable by bbb != 000:
const uint8_t DEY = 0b10001000;
//const uint8_t TAY = 0b10101000;
const uint8_t INY = 0b11001000;
const uint8_t INX = 0b11101000;
const uint8_t DEX = 0b11001010;
const uint8_t NOP = 0b11101010;

// Address mode irregularities can be kept at a minimum if we read them as follows:
// - JSR is absolute
// - LDY, CPY and CPX: all IMM versions must have bbb=000 (be wary of clash with 'regular' TAY construction)
// - STX, LDX has z,Y and a,Y modes instead of z,X / a,X


// Thanks to our pre-work, even most of the irregular mnemonics can
// now simply be ORed with their mode (which is often 'implied'). 
uint8_t encode(uint8_t mnem, uint8_t mode) {
     // mode encoding irregularities
    if (mnem == JSR) mode = 0;
    else if (mnem == LDY && mode == IMM) mode = 0;
    else if (mnem == CPY && mode == IMM) mode = 0;
    else if (mnem == CPX && mode == IMM) mode = 0;
    else if (mnem == LDX && mode == AY) mode = AX;

    // else: regular instruction, or mode = 0 for implied
    return mnem | mode;
}

// 'disco' example from 6502asm.com
uint8_t instructions[] = {
    INX,
    TXA,
    STA, AY, 0x00, 0x02,
    STA, AY, 0x00, 0x03,
    STA, AY, 0x00, 0x04,
    STA, AY, 0x00, 0x05,
    INY,
    TYA,
    CMP, IMM, 16, // NOTE wrong in original (zero page), but it still works; this number is irrelevant
    BRC, Z, 4, // jump 4 forward (skip jmp to start)
    INY,
    JMP, ABS, 0x00, 0x06, // = start
    INY,
    INY,
    INY,
    INY,
    JMP, ABS, 0x00, 0x06 // = start
};

void assemble(uint8_t * instructions, int n) {
    FILE * file = fopen("a.out", "w");
    int i=0;
    while (i<n) {
        uint8_t mnemonic = instructions[i++];

        // Work out if mode is 'implied' and therefore also implied in our code

        if (mnemonic == BRC || mnemonic == BRS ||
                ((mnemonic & 0b11100) == 0
                // && mnemonic != BRK && mnemonic != RTI && mnemonic != RTS // TODO differentiate between JMP and RTI et al. before knowing the mode
                )) {
            // Regular instruction, or at least apparent mode supplied by user
            // This also works for our branching syntax
            uint8_t mode = instructions[i++];
            fputc(encode(mnemonic, mode), file);

            // Infer number of args
            
            // NOTE: must supply ACC mode for ASL, ROL, LSR, ROR as there are also other modes
            // All other modes have at least 1 arg, so:
            if (!(mode == ACC && mnemonic & 0b11 == 0b10)) fputc(instructions[i++], file);
            // All ABS modes have 2 another arg:
            if (mode == ABS || mode == AX || mode == AY) fputc(instructions[i++], file);
        } else {
            // else assume implied; 0 args
            fputc(encode(mnemonic, 0), file);
        }
    }
}

int main (int argc, char ** argv) {
    assemble(instructions, sizeof(instructions) / sizeof(char));
}

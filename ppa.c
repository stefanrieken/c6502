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

// 65c02 only:
#define _TSB ((0b000 << 5) | 0b00)
#define _BIT ((0b001 << 5) | 0b00)
#define _JMP ((0b010 << 5) | 0b00)
#define _JPI ((0b011 << 5) | 0b00)
#define _STY ((0b100 << 5) | 0b00)
#define _LDY ((0b101 << 5) | 0b00)
#define _CPY ((0b110 << 5) | 0b00)
#define _CPX ((0b111 << 5) | 0b00)

#define _ORA ((0b000 << 5) | 0b01)
#define _AND ((0b001 << 5) | 0b01)
#define _EOR ((0b010 << 5) | 0b01)
#define _ADC ((0b011 << 5) | 0b01)
#define _STA ((0b100 << 5) | 0b01)
#define _LDA ((0b101 << 5) | 0b01)
#define _CMP ((0b110 << 5) | 0b01)
#define _SBC ((0b111 << 5) | 0b01)

#define _ASL ((0b000 << 5) | 0b10)
#define _ROL ((0b001 << 5) | 0b10)
#define _LSR ((0b010 << 5) | 0b10)
#define _ROR ((0b011 << 5) | 0b10)
#define _STX ((0b100 << 5) | 0b10)
#define _LDX ((0b101 << 5) | 0b10)
#define _DEC ((0b110 << 5) | 0b10)
#define _INC ((0b111 << 5) | 0b10)

// Clear / set instructions
#define _CLR  ((0b0110 << 2) | 0b00)
#define _SET  ((0b1110 << 2) | 0b00)
#define _CARY ((0b00 << 6))
#define _INTR ((0b01 << 6))
#define _OVFL ((0b10 << 6))
#define _BCD  ((0b11 << 6))

// Branch instructions
// Brach if clear
#define _BRC ((0b0100 << 2) | 0b00)
// Branch if set
#define _BRS ((0b1100 << 2) | 0b00)
// Registers use different encoding than with clear / set:
// Negative flag
#define _N (0b00 << 6)
// Overflow flag
#define _V (0b01 << 6)
// Carry flag
#define _C (0b10 << 6)
// Zero flag
#define _Z (0b11 << 6)

// Push / pull SR / Acc
#define _PSH (0b0010 << 2)
#define _POP (0b1010 << 2)
#define _SR  (0b00 << 6)
#define _AR  (0b01 << 6)

// The TXA, TSX family of functions can be synthesized as:
// TXA = STX | ACC; TSX = LDX | STACK; etc.
// TYA is still irregular (and TAY half); therefore
// it pays to just write these out:

#define _TXA (_STX | ACC | 0b10)
#define _TAX (_LDX | ACC | 0b10)
// Actually fully irregular:
#define _TYA 0b10011000
// Still quite irregular:
#define _TAY (_LDY | ACC | 0b00)
#define _TXS (_STX | STK | 0b10)
#define _TSX (_LDX | STK | 0b10)

// That leaves us with the fully irregular instructions.
// As with the alternative encodings above, we can detect
// these mnemonics by their bbb placeholder not being 000.

// The first 4 instructions in cc=00 are an exception:
#define _BRK (0b00000000)
#define _JSR (0b00100000)
#define _RTI (0b01000000)
#define _RTS (0b11000000)

// But these are all detectable by bbb != 000:
#define _DEY 0b10001000
//#define _TAY 0b10101000
#define _INY 0b11001000
#define _INX 0b11101000
#define _DEX 0b11001010
#define _NOP 0b11101010

// Enum mnemonics in alphabetical order
enum mnemonics {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
    CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
    JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
    RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA,
    NUM_MNEMONICS
};

// And use the same order for a lookup table
char lookuptable[] = {
    'A', 'D', 'C', _ADC, 'A', 'N', 'D', _AND, 'A', 'S', 'L', _ASL,
    'B', 'C', 'C', _BRC | _C, 'B', 'C', 'S', _BRS | _C, 'B', 'E', 'Q', _BRS | _Z,
    'B', 'I', 'T', _BIT,
    'B', 'M', 'I', _BRS | _N, 'B', 'N', 'E', _BRC | _Z,
    'B', 'P', 'L', _BRC | _N,
    'B', 'R', 'K', _BRK,
    'B', 'V', 'C', _BRC | _V, 'B', 'V', 'S', _BRS | _V,
    'C', 'L', 'C', _CLR | _CARY, 'C', 'L', 'D', _CLR | _BCD, 'C', 'L', 'I', _CLR | _INTR, 'C', 'L', 'V', _CLR | _OVFL,
    'C', 'M', 'P', _CMP, 'C', 'P', 'X', _CPX, 'C', 'P', 'Y', _CPY,
    'D', 'E', 'C', _DEC, 'D', 'E', 'X', _DEX, 'D', 'E', 'Y', _DEY,
    'E', 'O', 'R', _EOR,
    'I', 'N', 'C', _INC, 'I', 'N', 'X', _INX, 'I', 'N', 'Y', _INY,
    'J', 'M', 'P', _JMP, 'J', 'S', 'R', _JSR,
    'L', 'D', 'A', _LDA, 'L', 'D', 'X', _LDX, 'L', 'D', 'Y', _LDY, 'L', 'S', 'R', _LSR,
    'N', 'O', 'P', _NOP, 'O', 'R', 'A', _ORA,
    'P', 'H', 'A', _PSH | _AR, 'P', 'H', 'P', _PSH | _SR, 'P', 'L', 'A', _POP | _AR, 'P', 'L', 'P', _POP | _SR,
    'R', 'O', 'L', _ROL, 'R', 'O', 'R', _ROR, 'R', 'T', 'I', _RTI, 'R', 'T', 'S', _RTS,
    'S', 'B', 'C', _SBC,
    'S', 'E', 'C', _SET | _CARY, 'S', 'E', 'D', _SET | _BCD, 'S', 'E', 'I', _SET | _INTR,
    'S', 'T', 'A', _STA, 'S', 'T', 'X', _STX, 'S', 'T', 'Y', _STY,
    'T', 'A', 'X', _TAX, 'T', 'A', 'Y', _TAY, 'T', 'S', 'X', _TSX, 'T', 'X', 'A', _TXA, 'T', 'X', 'S', _TXS, 'T', 'Y', 'A', _TYA
};

// Address mode irregularities can be kept at a minimum if we read them as follows:
// - JSR is absolute
// - LDY, CPY and CPX: all IMM versions must have bbb=000 (be wary of clash with 'regular' TAY construction)
// - STX, LDX has z,Y and a,Y modes instead of z,X / a,X

// Thanks to our pre-work, even most of the irregular mnemonics can
// now simply be ORed with their mode (which is often 'implied'). 
uint8_t encode(uint8_t mnem, uint8_t template, uint8_t mode) {
     // mode encoding irregularities
    if (mnem == JSR) mode = 0;
    else if (mnem == LDY && mode == IMM) mode = 0;
    else if (mnem == CPY && mode == IMM) mode = 0;
    else if (mnem == CPX && mode == IMM) mode = 0;
    else if (mnem == LDX && mode == AY) mode = AX;

    // else: regular instruction, or mode = 0 for implied
    return template | mode;
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
    BNE, 4, // jump 4 forward (skip jmp to start)
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
        // The instructions array refers to the mnemonics by their enum values.
	// This allows us to distinguish mnemonics that end up having the same binary-value template.
	// (Which really only occurs near BRK, JSR, RTI and RTS, but should be avoided anyway.)
        uint8_t mnemonic = instructions[i++];

        // Work out if mode is 'implied' and therefore also implied in our code
        uint8_t template = lookuptable[mnemonic*4+3];

	if ((template & 0b11111) == 0b10000) { // Branch instruction
            // No explicit mode follows, but we do take a relative argument
	    fputc(encode(mnemonic, template, 0), file);
	    fputc(instructions[i++], file);
        } else if ((template & 0b11100) == 0 // No mode given yet, but check for incidental mode==000:
                && mnemonic != BRK && mnemonic != RTI && mnemonic != RTS
                ) {
            // Regular instruction, or at least apparent mode supplied by user
            // This also works for our branching syntax
            uint8_t mode = instructions[i++];
            fputc(encode(mnemonic, template, mode), file);

            // Infer number of args
            
            // NOTE: must supply ACC mode for ASL, ROL, LSR, ROR as there are also other modes
            // All other modes have at least 1 arg, so:
            if (!(mode == ACC && mnemonic & 0b11 == 0b10)) fputc(instructions[i++], file);
            // All ABS modes have 2 another arg:
            if (mode == ABS || mode == AX || mode == AY) fputc(instructions[i++], file);
        } else {
            // else assume implied; 0 args
            fputc(encode(mnemonic, template, 0), file);
        }
    }
}

int main (int argc, char ** argv) {
    printf("Sizeof enum: %d sizeof lookuptable: %d sta: %c%c%c\n", NUM_MNEMONICS, sizeof(lookuptable), lookuptable[(STA*4)+0], lookuptable[(STA*4)+1], lookuptable[(STA*4)+2]);
    assemble(instructions, sizeof(instructions) / sizeof(char));
}

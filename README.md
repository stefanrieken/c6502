# c6502

This is a 6502 simulator written in C.

This project replicates the display function of the now defunct 6502asm.com,
wich in turn seems to have been based on 6502js. Copies of both sites can still
be found at
[http://daeken.github.io/Dyna6502Asm/](http://daeken.github.io/Dyna6502Asm/)
and [https://itema-as.github.io/6502js/](https://itema-as.github.io/6502js/),
respectively.

More code examples copied from 6502asm.com can be found at my earlier
Java-based emulator,
[here](https://github.com/stefanrieken/simple64/tree/master/src/test/resources).

c6502 implements the 32x32 display using GTK+ 3. The keyboard is not yet
implemented.

c6502 now has full (untested) support for the WDC65c02 extensions if compiled
with `-DWDC65c02`. (As the 65c02 is fully backwards compatible for well-defined
instructions, the flag is mainly used to mark 65c02 extensions in code, and / or
to save a tiny amount of overhead.)

## 6502 instruction encoding
Thanks to: [https://llx.com/Neil/a2/opcodes.html](https://llx.com/Neil/a2/opcodes.html)

6502 opcodes can be split into `aaabbbcc`, where `cc` defines an opcode group,
`aaa` an opcode within the group, and `bbb` an addressing mode.

In the tables below, these regular opcodes (`aaa`) are labeled on the columns,
and the address modes (`bbb`) on the rows. A blank cell implies that both
column and row data combine into a regular function; but a filled cell instead
indicates an irregular instruction.

For example, the position `001 000 00` indeed contains a regular `BIT #`
instruction; on the other hand, the neigbouring `BRK` instruction at
`000 000 00` is irregular and therefore explicitly filled in. This approach
quickly visualises the regularities and irregularities of the encoding.

### Addressing modes

        000 #immediate OR (zero page, X) on cc = 01
        001 Zero page
        010 accumulator OR #immediate on cc = 01
        011 absolute
        100 (zero page), Y
        101 zero page, X
        110 absolute, Y
        111 absolute, X

### Branching
Conditional branches follow the pattern `xxy10000`, where `xx` specifies a flag
to test, and `y` whether to branch on positive or negative. This separate pattern
places all conditionals in `cc=00`, `bbb=100`.

### Later additions
Opcode group `11` has always been preserved for future / coprocessor use. Some
later variants use this group; the WDC65c815 for 24-bit variants of group `01`.

Here we only document the additions of the 65c02, as these can be seen to fill
most of the encoding gaps left by the original 6502.

### cc = 00

                    000   001   010   011   100   101   110   111
                    TSB*  BIT   JMP   JMP() STY   LDY   CPY   CPX   * = Opcode added in 65c02
        000 #       BRK   JSRa  RTI   RTS   BRA*                    * = Instr added in 65c02
        001 zp                  -     STZ*                          * = Instr added in 65c02
        010 -       PHP   PLP   PHA   PLA   DEY   TAY   INY   INX
        011 abs
        100 -       BPL   BMI   BVC   BVS   BCC   BCS   BNE   BEQ   <-- All the branch instructions
        101 z,X     TRBz* *     -     STZzx*            -     -     * = (Regular) instr added in 65c02
        110 -       CLC   SEC   CLI   SEI   TYA   CLV   CLD   SED   <-- All the flag instructions + TYA
        111 a,X     TRBa* *     -     *     STZa*       -     -     * = (Regular) instr added in 65c02

### cc = 01

                    000   001   010   011   100   101   110   111
                    ORA   AND   EOR   ADC   STA   LDA   CMP   SBC
        000 (z,X)
        001 zp
        010 #
        011 abs                             BIT#*                   * = Instr added in 65c02
        100 (z),Y
        101 z,X
        110 a,Y
        111 a,X

### cc = 10

                    000   001   010   011   100   101   110   111
                    ASL   ROL   LSR   ROR   STX   LDX   DEC   INC
        000 #       -     -     -     -     -           -     -     <-- In practice, some are 2-cycle NOPs
        001 zp
        010 acc                             TXA   TAX   DEX   NOP
        011 abs                             -     -     -     -
        100 (zp)*   ORA   AND   EOR   ADC   STA   LDA   CMP   SBC   <-- 65c02 only: new (zp) mode
        101 z,X                             z,Y   z,Y               <-- Address mode exception
        110 -       INCa* DECa* PHY*  PLY*  TXS   TSX   PHX*  PLX*  * = Instr added in 65c02
        111 a,X                             STZy* a,Y               <-- Added in 65c02 / Address mode exception


## Approaching the (ir)regularity
A close look reveals that the majority of the irregular instructions and
vacancies are found at `cc= x0, bbb = xx0`, and that the few `bbb = xx1`
exceptions are mostly useless or unsupported regular forms.

For the most part, the 65c02 neatly fills in these "regular irregularites",
with only a handful of `bbb = 01` exceptions.

For this reason, when decoding we can 'overlay' the irregular even-row pattern
before trying a regular-encoding lookup. As for the handful of 65c02 odd-row
exceptions, we'll just have to check for these by hand first thing.

Address mode exceptions can be handled by the individual instruction (as with
STX / LDX indexed by Y), or caught earlier / independently if it affects a
wider group (as with the `bbb = 000` per-page differences).


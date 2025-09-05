// Most instructions would only require bbb, but I am not in a hurry to change the signature of 56 functions.
typedef void (* Instruction)(uint8_t aaa, uint8_t bbb, uint8_t cc);

// 24 regular instructions (with TSB being 65c02 only)
void tsb(uint8_t aaa, uint8_t bbb, uint8_t cc); void bit(uint8_t aaa, uint8_t bbb, uint8_t cc); void jmp(uint8_t aaa, uint8_t bbb, uint8_t cc); void jp2(uint8_t aaa, uint8_t bbb, uint8_t cc);
void sty(uint8_t aaa, uint8_t bbb, uint8_t cc); void ldy(uint8_t aaa, uint8_t bbb, uint8_t cc); void cpy(uint8_t aaa, uint8_t bbb, uint8_t cc); void cpx(uint8_t aaa, uint8_t bbb, uint8_t cc);
void ora(uint8_t aaa, uint8_t bbb, uint8_t cc); void and(uint8_t aaa, uint8_t bbb, uint8_t cc); void eor(uint8_t aaa, uint8_t bbb, uint8_t cc); void adc(uint8_t aaa, uint8_t bbb, uint8_t cc);
void sta(uint8_t aaa, uint8_t bbb, uint8_t cc); void lda(uint8_t aaa, uint8_t bbb, uint8_t cc); void cmp(uint8_t aaa, uint8_t bbb, uint8_t cc); void sbc(uint8_t aaa, uint8_t bbb, uint8_t cc);
void asl(uint8_t aaa, uint8_t bbb, uint8_t cc); void rol(uint8_t aaa, uint8_t bbb, uint8_t cc); void lsr(uint8_t aaa, uint8_t bbb, uint8_t cc); void ror(uint8_t aaa, uint8_t bbb, uint8_t cc);
void stx(uint8_t aaa, uint8_t bbb, uint8_t cc); void ldx(uint8_t aaa, uint8_t bbb, uint8_t cc); void dec(uint8_t aaa, uint8_t bbb, uint8_t cc); void inc(uint8_t aaa, uint8_t bbb, uint8_t cc);

// 32 irregular instructions (but some are re-used over different opcodes; and some are 65c02 only and / or re-use regular instructions)
void brk(uint8_t aaa, uint8_t bbb, uint8_t cc); void jsr(uint8_t aaa, uint8_t bbb, uint8_t cc); void rti(uint8_t aaa, uint8_t bbb, uint8_t cc); void rts(uint8_t aaa, uint8_t bbb, uint8_t cc);
void bra(uint8_t aaa, uint8_t bbb, uint8_t cc); void php(uint8_t aaa, uint8_t bbb, uint8_t cc); void plp(uint8_t aaa, uint8_t bbb, uint8_t cc); void pha(uint8_t aaa, uint8_t bbb, uint8_t cc);
void pla(uint8_t aaa, uint8_t bbb, uint8_t cc); void dey(uint8_t aaa, uint8_t bbb, uint8_t cc); void tay(uint8_t aaa, uint8_t bbb, uint8_t cc); void iny(uint8_t aaa, uint8_t bbb, uint8_t cc);
void inx(uint8_t aaa, uint8_t bbb, uint8_t cc); void br_(uint8_t aaa, uint8_t bbb, uint8_t cc); void c_s(uint8_t aaa, uint8_t bbb, uint8_t cc); void tya(uint8_t aaa, uint8_t bbb, uint8_t cc);
void txa(uint8_t aaa, uint8_t bbb, uint8_t cc); void tax(uint8_t aaa, uint8_t bbb, uint8_t cc); void dex(uint8_t aaa, uint8_t bbb, uint8_t cc); void nop(uint8_t aaa, uint8_t bbb, uint8_t cc);
void ora(uint8_t aaa, uint8_t bbb, uint8_t cc); void and(uint8_t aaa, uint8_t bbb, uint8_t cc); void eor(uint8_t aaa, uint8_t bbb, uint8_t cc); void adc(uint8_t aaa, uint8_t bbb, uint8_t cc);
void inc(uint8_t aaa, uint8_t bbb, uint8_t cc); void dec(uint8_t aaa, uint8_t bbb, uint8_t cc); void phy(uint8_t aaa, uint8_t bbb, uint8_t cc); void ply(uint8_t aaa, uint8_t bbb, uint8_t cc);
void txs(uint8_t aaa, uint8_t bbb, uint8_t cc); void tsx(uint8_t aaa, uint8_t bbb, uint8_t cc); void phx(uint8_t aaa, uint8_t bbb, uint8_t cc); void plx(uint8_t aaa, uint8_t bbb, uint8_t cc);

// 2 more higly irregular 65c02 functions; these (and BIT imm) may prefer bbb to be normalized before calling
void trb(uint8_t aaa, uint8_t bbb, uint8_t cc); void stz(uint8_t aaa, uint8_t bbb, uint8_t cc);


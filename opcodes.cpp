#include "system.h"

opcode::opcode(uint8_t ocode, const char * ocodestr)
{
    code = ocode;
    opcodestr = ocodestr;
}

void opcode::printcode()
{
    printf("%x\n", code);
}

const char * addopcode(std::map<uint8_t, std::shared_ptr<opcode> > * curmap, uint8_t code, const char * inststr)
{
    /*
     * creates a new opcode in the opcode map
     * returns the string passed in
     * NOTE: I'm not sure if this method will leak the old contents of memory or not if already assigned...
     * cursory research suggests no
     */
    (*curmap)[code] = std::shared_ptr<opcode>(new opcode(code, inststr));
    return inststr;
}

std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate * cpu, Memory * mem)
{
    /*
     * todo: implement:
     * ROR, ASL, LSR, ROL
     * PLP, PLA, PHP, PHA
     * ORA, EOR, AND
     * NOP
     * LDX, LDY
     * JSR, JMP, BVS, BVC, BPL, BNE, BMI, BEQ, BCS, BCC - jumping and branching, last to do
     * DEY, DEX, DEC
     * CPY, CPX, CMP
     * CLV, CLD
     * BIT
     */
    // http://obelisk.me.uk/6502/reference.html

    std::map<uint8_t, std::shared_ptr<opcode> > opmap;
    const char * myasm;

    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(uint8_t i = 0x0; i < 0xff; i++)
    {
        addopcode(&opmap, i, "test");
        opmap[i]->f = [i, cpu]() {
            printf("%x-", i);

            cpu->pc += 1;
            return 1;
        };
    }

    // todo: unit test all opcodes
    // http://www.emulator101.com/reference/6502-reference.html

    // note: all opcode functions return number of cpu cycles to execute and advance the program counter the appropriate number of bytes

    // BRK
    myasm = addopcode(&opmap, 0x00, "0x00 BRK");
    opmap[0x00]->f = [cpu, myasm]() {
        printf("\n%s\n", myasm);

        // todo: generates interrupts, implement properly

        cpu->pc += 1;
        return 7;
    };

    // CLC 0x18
    myasm = addopcode(&opmap, 0x18, "0x18 clc");
    opmap[0x18]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);

        // clear carry
        cpu->p &= ~(1 << 7);

        cpu->pc += 1;
        return 2;
    };

    // SEC 0x38
    myasm = addopcode(&opmap, 0x38, "0x38 sec");
    opmap[0x38]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);

        // set carry
        cpu->p |= (1 << 7);

        cpu->pc += 1;
        return 2;
    };

    // CLI 0x58
    myasm = addopcode(&opmap, 0x58, "0x58 cli");
    opmap[0x58]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);

        // clear interrupt disable flag
        cpu->p &= ~(1 << 5);

        cpu->pc += 1;
        return 2;
    };

    // ADC indexed indirect
    myasm = addopcode(&opmap, 0x61, "0x61 adc ($");
    opmap[0x61]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        adc(cpu, indexedindirect(cpu, mem));

        cpu->pc += 2;
        return 6;
    };

    // ADC zero page
    myasm = addopcode(&opmap, 0x65, "0x65 adc $");
    opmap[0x65]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        adc(cpu, zeropage(cpu, mem));

        cpu->pc += 2;
        return 3;
    };

    // ADC immediate
    myasm = addopcode(&opmap, 0x69, "0x69 adc #$");
    opmap[0x69]->f = [cpu, mem, myasm]() {
        uint8_t val = mem->readmem(cpu->pc+1);
        if (DEBUG) printf("\n%s%x\n", myasm, val);
        adc(cpu, val);

        cpu->pc += 2;
        return 2;
    };

    // ADC absolute $6d
    myasm = addopcode(&opmap, 0x6d, "0x6d adc $");
    opmap[0x6d]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        adc(cpu, mem->readmem(addr));

        cpu->pc += 3;
        return 4;
    };

    // ADC indirect indexed $71
    myasm = addopcode(&opmap, 0x71, "0x71 adc ($");
    opmap[0x71]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        adc(cpu, mem->readmem(indirectindexed(cpu, mem)));

        cpu->pc += 2;
        return 5;
        // todo: +1 if page crossed
    };

    // ADC zero page,x $75
    myasm = addopcode(&opmap, 0x75, "0x75 adc $");
    opmap[0x75]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        adc(cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // SEI 0x78
    myasm = addopcode(&opmap, 0x78, "0x78 sei");
    opmap[0x78]->f = [cpu, myasm] () {
        if(DEBUG) printf("\n%s\n", myasm);

        // set interrupt disable flag
        cpu->p |= (1 << 5);

        cpu->pc += 1;
        return 2;
    };

    // ADC absolute,y $79
    myasm = addopcode(&opmap, 0x79, "0x79 adc $");
    opmap[0x79]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        adc(cpu, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // ADC absolute,x $7d
    myasm = addopcode(&opmap, 0x7d, "0x7d adc $");
    opmap[0x7d]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        adc(cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // STA indexed indirect
    myasm = addopcode(&opmap, 0x81, "0x81 sta ($");
    opmap[0x81]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        sta(cpu, mem, indexedindirect(cpu, mem));

        cpu->pc += 2;
        return 6;
    };

    // STY zero page 0x84
    myasm = addopcode(&opmap, 0x84, "0x84 sty $");
    opmap[0x84]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        sty(cpu, mem, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
        return 3;
    };

    // STA zero page
    myasm = addopcode(&opmap, 0x85, "0x85 sta $");
    opmap[0x85]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        sta(cpu, mem, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
        return 3;
    };

    // STX zero page 0x86
    myasm = addopcode(&opmap, 0x86, "0x86 stx $");
    opmap[0x86]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        stx(cpu, mem, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
        return 3;
    };

    // TXA 0x8a
    myasm = addopcode(&opmap, 0x8a, "0x8a txa");
    opmap[0x8a]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        cpu->a = cpu->x;
        setzeroflag(cpu->a, cpu);
        setnegflag(cpu->a, cpu);

        cpu->pc += 1;
        return 2;
    };

    // STY absolute 0x8c
    myasm = addopcode(&opmap, 0x8c, "0x8c sty $");
    opmap[0x8c]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        // I don't think previous method of absolute addressing is correct
        // we now write directory to addr address, not the address read from there
        // should be correct now, but also check absolute,x and absolute,y (I think they are correct)
        // all in all - todo: unit test
        sty(cpu, mem, addr);

        cpu->pc += 3;
        return 4;
    };

    // STA absolute
    myasm = addopcode(&opmap, 0x8d, "0x8d sta $");
    opmap[0x8d]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        sta(cpu, mem, addr);

        cpu->pc += 3;
        return 4;
    };

    // STX absolute 0x8e
    myasm = addopcode(&opmap, 0x8e, "0x8e stx $");
    opmap[0x8e]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        stx(cpu, mem, addr);

        cpu->pc += 3;
        return 4;
    };

    // STA indirect indexed
    myasm = addopcode(&opmap, 0x91, "0x91 sta ($");
    opmap[0x91]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        sta(cpu, mem, mem->readmem(indirectindexed(cpu, mem)));

        cpu->pc += 2;
        return 6;
    };

    // STY zero page,x 0x94
    myasm = addopcode(&opmap, 0x94, "0x94 sty $");
    opmap[0x94]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        sty(cpu, mem, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // STA zero page,x
    myasm = addopcode(&opmap, 0x95, "0x95 sta $");
    opmap[0x95]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        sta(cpu, mem, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // STX zero page,y 0x96
    myasm = addopcode(&opmap, 0x96, "0x96 stx $");
    opmap[0x96]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,Y\n", myasm, mem->readmem(cpu->pc+1));
        stx(cpu, mem, mem->readmem(zeropagey(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // TYA 0x98
    myasm = addopcode(&opmap, 0x98, "0x98 tya");
    opmap[0x98]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        cpu->a = cpu->y;
        setzeroflag(cpu->a, cpu);
        setnegflag(cpu->a, cpu);

        cpu->pc += 1;
        return 2;
    };

    // STA absolute,y
    myasm = addopcode(&opmap, 0x99, "0x99 sta $");
    opmap[0x99]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        sta(cpu, mem, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 5;
    };

    // TXS 0x9a
    myasm = addopcode(&opmap, 0x9a, "0x9a txs");
    opmap[0x9a]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        cpu->sp = cpu->x;

        cpu->pc += 1;
        return 2;
    };

    // STA absolute,x
    myasm = addopcode(&opmap, 0x9d, "0x9d sta $");
    opmap[0x9d]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        sta(cpu, mem, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 5;
    };

    // STA zero page,y??? does it exist? nope

    // LDA
    // indexed indirect address mode - slightly convoluted
    // http://www.emulator101.com/6502-addressing-modes.html
    myasm = addopcode(&opmap, 0xa1, "0xa1 lda ($");
    opmap[0xa1]->f = [cpu, mem, myasm]() {
        // todo: the readmem method returns a uint8_t, should it return uint16_t?
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        lda(cpu, mem->readmem(indexedindirect(cpu, mem)));

        cpu->pc += 2;
        return 6;
    };

    // LDA zero page
    myasm = addopcode(&opmap, 0xa5, "0xa5 lda $");
    opmap[0xa5]->f = [cpu, mem, myasm]() {
        // todo: UNTESTED
        if(DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        // I think passing a uint8_t to a uint16_t should work without issue
        lda(cpu, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
        return 3;
    };

    // TAY 0xa8
    myasm = addopcode(&opmap, 0xa8, "0xa8 tay");
    opmap[0xa8]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        cpu->y = cpu->a;
        setzeroflag(cpu->y, cpu);
        setnegflag(cpu->y, cpu);

        cpu->pc += 1;
        return 2;
    };

    // LDA immediate
    myasm = addopcode(&opmap, 0xa9, "0xa9 lda #$");
    opmap[0xa9]->f = [cpu, mem, myasm]() {
        uint8_t val = mem->readmem(cpu->pc+1);
        if(DEBUG) printf("\n%s%x\n", myasm, val);
        lda(cpu, val);

        cpu->pc += 2;
        return 2;
    };

    // TAX 0xaa
    myasm = addopcode(&opmap, 0xaa, "0xaa tax");
    opmap[0xaa]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        cpu->x = cpu->a;
        setzeroflag(cpu->x, cpu);
        setnegflag(cpu->x, cpu);

        cpu->pc += 1;
        return 2;
    };

    // LDA $0314 constant
    // absolute address mode
    myasm = addopcode(&opmap, 0xad, "0xad lda $");
    opmap[0xad]->f = [cpu, mem, myasm]() {
        /*
         * ok, so this is absolute addressing, 3 byte instruction
         * the two bytes after 0xad specify an exact addr to get data to put in A from
         * need to reverse for little endian
         */

        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        lda(cpu, mem->readmem(addr));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);

        cpu->pc += 3;
        return 4;
    };

    // LDA indirect indexed
    myasm = addopcode(&opmap, 0xb1, "0xb1 lda ($");
    opmap[0xb1]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        lda(cpu, mem->readmem(indirectindexed(cpu, mem)));

        cpu->pc += 2;
        return 5;
        // todo: +1 if page crossed
    };

    // LDA zero page,X
    myasm = addopcode(&opmap, 0xb5, "0xb5 lda $");
    opmap[0xb5]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        lda(cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // LDA absolute,Y
    myasm = addopcode(&opmap, 0xb9, "0xb9 lda $");
    opmap[0xb9]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        lda(cpu, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // 0xba TSX
    myasm = addopcode(&opmap, 0xba, "0xba tsx");
    opmap[0xba]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        cpu->x = cpu->sp;
        setzeroflag(cpu->x, cpu);
        setnegflag(cpu->x, cpu);

        cpu->pc += 1;
        return 2;
    };

    // 0xbd LDA absolute,X
    myasm = addopcode(&opmap, 0xbd, "0xbd lda $");
    opmap[0xbd]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        lda(cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // INY 0xc8
    myasm = addopcode(&opmap, 0xc8, "0xc8 iny");
    opmap[0xc8]->f = [cpu, myasm]() {
        if (DEBUG) printf("\n%s\n", myasm);

        cpu->y++;

        setzeroflag(cpu->y, cpu);
        setnegflag(cpu->y, cpu);

        cpu->pc += 1;
        return 2;
    };

    /*
    SBC #$NN	Immediate	$e9	CZ- - - VN
    SBC $NNNN	Absolute	$ed	CZ- - - VN
    SBC $NNNN,X	Absolute,X	$fd	CZ- - - VN
    SBC $NNNN,Y	Absolute,Y	$f9	CZ- - - VN
    SBC $NN	Zero Page	$e5	CZ- - - VN
    SBC $NN,X	Zero Page,X	$f5	CZ- - - VN
    SBC ($NN,X)	Indexed Indirect	$e1	CZ- - - VN
    SBC ($NN),Y	Indirect Indexed	$f1	CZ- - - VN
     */
    // SBC zero page
    myasm = addopcode(&opmap, 0xe5, "0xe5 sbc $");
    opmap[0xe5]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        sbc(cpu, zeropage(cpu, mem));

        cpu->pc += 2;
        return 3;
    };

    // INC zero page 0xe6
    myasm = addopcode(&opmap, 0xe6, "0xe6 inc $");
    opmap[0xe6]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        inc(cpu, mem, mem->readmem(cpu->pc + 1));

        cpu->pc += 2;
        return 5;
    };

    // INX (implied addressing mode 0xe8
    myasm = addopcode(&opmap, 0xe8, "0xe8 inx");
    opmap[0xe8]->f = [cpu, myasm]() {
        if (DEBUG) printf("\n%s\n", myasm);

        cpu->x++;

        setzeroflag(cpu->x, cpu);
        // todo: I wonder if just using a regular uint8_t is going to mess with our negative flag...
        setnegflag(cpu->x, cpu);

        cpu->pc += 1;
        return 2;
    };

    // SBC immediate
    myasm = addopcode(&opmap, 0xe9, "0xe9 sbc #$");
    opmap[0xe9]->f = [cpu, mem, myasm]() {
        uint8_t val = mem->readmem(cpu->pc+1);
        if (DEBUG) printf("\n%s%x\n", myasm, val);
        sbc(cpu, val);

        cpu->pc += 2;
        return 2;
    };

    // SBC absolute $ed
    myasm = addopcode(&opmap, 0xed, "0xed sbc $");
    opmap[0xed]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        adc(cpu, mem->readmem(addr));

        cpu->pc += 3;
        return 4;
    };

    // INC absolute $ee
    myasm = addopcode(&opmap, 0xee, "0xee inc $");
    opmap[0xee]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2));
        if (DEBUG) printf("\n%s%x\n", myasm, addr);
        inc(cpu, mem, addr);

        cpu->pc += 3;
        return 6;
    };

    // SBC indirect indexed $f1
    myasm = addopcode(&opmap, 0xf1, "0xf1 sbc ($");
    opmap[0xf1]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        sbc(cpu, mem->readmem(indirectindexed(cpu, mem)));

        cpu->pc += 2;
        return 5;
        // todo: +1 if page crossed
    };

    // SBC zero page,x $f5
    myasm = addopcode(&opmap, 0xf5, "0xf5 sbc $");
    opmap[0xf5]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        sbc(cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // INC zero page,x $f6
    myasm = addopcode(&opmap, 0xf6, "0xf6 inc $");
    opmap[0xf6]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, mem->readmem(cpu->pc + 1));
        inc(cpu, mem, zeropagex(cpu, mem));

        cpu->pc += 2;
        return 6;
    };

    // SED 0xf8 - shouldn't be used on NES, but apparently is?
    myasm = addopcode(&opmap, 0xf8, "0xf8 sed");
    opmap[0xf8]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);

        // set decimal mode flag
        cpu->p |= (1 << 4);

        cpu->pc += 1;
        return 2;
    };

    // SBC absolute,y $f9
    myasm = addopcode(&opmap, 0xf9, "0xf9 sbc $");
    opmap[0xf9]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        sbc(cpu, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // SBC absolute,x $fd
    myasm = addopcode(&opmap, 0xfd, "0xfd sbc $");
    opmap[0xfd]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        sbc(cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo +1 if page crossed
    };

    // INC absolute,x $fe
    myasm = addopcode(&opmap, 0xfe, "0xfe inc $");
    opmap[0xfe]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        inc(cpu, mem, absolutex(cpu, mem));

        cpu->pc += 3;
        return 7;
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    myasm = addopcode(&opmap, 0xff, "DNE");
    opmap[0xff]->f = [cpu]() { printf("\n0xff DNE\n"); cpu->pc += 1; return 1; };

    return opmap;
}
#include "system.h"

/*
 * https://ascii.cl/conversion.htm
 * Well... I think this is fundamentally flawed now...
 * If I'm correct:
 * Each opcode is a byte, composed of 8 bits.
 * This byte is the input to the processor, and each bit goes to a different unit of the processor
 * Therefore something like changing from addressing mode zero page to zero page,x
 * would be as simple as turning on the fifth bit in 11000110 to get 11010110 (0xC6 to 0xD6)
 *
 * I might be totally wrong, but that would make sense... need to look at the opcode relationships
 * They seem to have relationships to each other where they do things like increase by 8 or f
 * Hmmmmmm
 */

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
     * ROR, ROL
     * JSR, JMP, BVS, BVC, BPL, BNE, BMI, BEQ, BCS, BCC - jumping and branching, last to do
     * BRK
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

    // ORA 0x01 indirect,x
    myasm = addopcode(&opmap, 0x1, "0x01 ora ($");
    opmap[0x1]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        ORA(cpu, indexedindirect(cpu, mem));

        cpu->pc += 2;
        return 6;
    };

    // ORA zero page 0x05
    myasm = addopcode(&opmap, 0x5, "0x05 ora $");
    opmap[0x5]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        ORA(cpu, zeropage(cpu, mem));

        cpu->pc += 2;
        return 3;
    };

    // ASL zero page 0x06
    myasm = addopcode(&opmap, 0x6, "0x06 asl $");
    opmap[0x6]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        aslmem(cpu, mem, zeropage(cpu, mem));

        cpu->pc += 2;
        return 5;
    };

    // PHP 0x08
    myasm = addopcode(&opmap, 0x8, "0x08 php");
    opmap[0x8]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s\n", myasm);
        push(cpu, mem, cpu->p);

        cpu->pc += 1;
        return 3;
    };

    // ORA immedate 0x09
    myasm = addopcode(&opmap, 0x9, "0x09 ora #$");
    opmap[0x9]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        ORA(cpu, mem->readmem(cpu->pc+1));

        cpu->pc += 2;
        return 2;
    };

    // ASL accumulator 0x0a (1, 2)
    myasm = addopcode(&opmap, 0x4a, "0x0a asl A");
    opmap[0xa]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        asla(cpu, &cpu->a);

        cpu->pc += 1;
        return 2;
    };

    // ORA absolute 0x0d (3, 4)
    myasm = addopcode(&opmap, 0xd, "0x0d ora $");
    opmap[0xd]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        ORA(cpu, mem->readmem(addr));

        cpu->pc += 3;
        return 4;
    };

    // ASL absolute 0x0e
    myasm = addopcode(&opmap, 0xe, "0x0e asl $");
    opmap[0xe]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        aslmem(cpu, mem, addr);

        cpu->pc += 3;
        return 6;
    };

    // ORA 0x11 indirect,y
    myasm = addopcode(&opmap, 0x11, "0x11 ora ($");
    opmap[0x11]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        ORA(cpu, indirectindexed(cpu, mem));

        cpu->pc += 2;
        return 5;   // todo: +1 if page crossed
    };


    // ORA zero page,x (2 bytes, 4 cycles) 0x15
    myasm = addopcode(&opmap, 0x15, "0x15 ora $");
    opmap[0x15]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, mem->readmem(cpu->pc + 1));
        ORA(cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // ASL zero page,x (2, 6) 0x16
    myasm = addopcode(&opmap, 0x16, "0x16 asl $");
    opmap[0x16]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n", myasm, mem->readmem(cpu->pc + 1));
        aslmem(cpu, mem, zeropagex(cpu, mem));

        cpu->pc += 2;
        return 6;
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

    // ORA absolute,x 0x1D
    myasm = addopcode(&opmap, 0x1d, "0x1d ora $");
    opmap[0x1d]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        ORA(cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4; // todo: +1 if page crossed
    };

    // ASL absolute,x 0x1e
    myasm = addopcode(&opmap, 0x1e, "0x1e asl $");
    opmap[0x1e]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        aslmem(cpu, mem, absolutex(cpu, mem));

        cpu->pc += 3;
        return 7;
    };

    // TODO: reorder this
    // ORA absolute,y 0x19
    myasm = addopcode(&opmap, 0x19, "0x19 ora $");
    opmap[0x19]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        ORA(cpu, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 4; // todo: +1 if page crossed
    };

    // AND indirect,x 0x21
    myasm = addopcode(&opmap, 0x21, "0x21 and ($");
    opmap[0x21]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        AND(cpu, indexedindirect(cpu, mem));

        cpu->pc += 2;
        return 6;
    };

    // BIT zero page 0x24
    myasm = addopcode(&opmap, 0x24, "0x24 bit $");
    opmap[0x24]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        bit(cpu, mem, zeropage(cpu, mem));

        cpu->pc += 2;
        return 3;
    };

    // AND zero page 0x25
    myasm = addopcode(&opmap, 0x25, "0x25 and $");
    opmap[0x25]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        // todo: AND expects a value, this was passing an address (no readmem around zeropage call), check other AND opcodes
        AND(cpu, mem->readmem(zeropage(cpu, mem)));
        // flags are set in our AND() function

        cpu->pc += 2;
        return 3;
    };

    // PLP 0x28
    myasm = addopcode(&opmap, 0x28, "0x28 plp");
    opmap[0x28]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s\n", myasm);
        cpu->p = pop(cpu, mem);

        cpu->pc += 1;
        return 4;
    };

    // AND immediate 0x29
    myasm = addopcode(&opmap, 0x29, "0x29 and #$");
    opmap[0x29]->f = [cpu, mem, myasm]() {
        // ok, immediate addressing specifies a constant in the code
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        AND(cpu, mem->readmem(cpu->pc+1));

        cpu->pc += 2;
        return 2;
    };

    // BIT absolute 0x2c
    myasm = addopcode(&opmap, 0x2c, "0x2c bit $");
    opmap[0x2c]->f = [cpu, mem, myasm] () {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        bit(cpu, mem, addr);

        cpu->pc += 3;
        return 4;
    };

    // AND absolute 0x2d
    myasm = addopcode(&opmap, 0x2d, "0x2d and $");
    opmap[0x2d]->f = [cpu, mem, myasm] () {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        AND(cpu, mem->readmem(addr));

        cpu->pc += 3;
        return 4;
    };

    // AND indirect,y 0x31
    myasm = addopcode(&opmap, 0x31, "0x31 and ($");
    opmap[0x31]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        AND(cpu, indirectindexed(cpu, mem));

        cpu->pc += 2;
        return 5;   // todo: +1 if page crossed
    };

    // AND zero page,x 0x35
    myasm = addopcode(&opmap, 0x35, "0x35 and $");
    opmap[0x35]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, mem->readmem(cpu->pc + 1));
        AND(cpu, mem->readmem(zeropagex(cpu, mem)));
        // flags are set in our AND() function

        cpu->pc += 2;
        return 4;
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

    // AND absolute,y 0x39
    myasm = addopcode(&opmap, 0x39, "0x39 and $");
    opmap[0x39]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        AND(cpu, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 4; // todo: +1 if page crossed
    };

    // AND absolute,x 0x3d
    myasm = addopcode(&opmap, 0x3d, "0x3d and $");
    opmap[0x3d]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        AND(cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4; // todo: +1 if page crossed
    };

    // EOR indirect,x 0x41
    myasm = addopcode(&opmap, 0x41, "0x41 eor ($");
    opmap[0x41]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        EOR(cpu, mem->readmem(indexedindirect(cpu, mem)));

        cpu->pc += 2;
        return 6;
    };

    // EOR zero page 0x45 (2, 3)
    myasm = addopcode(&opmap, 0x45, "0x45 eor $");
    opmap[0x45]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));

        EOR(cpu, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
        return 3;
    };

    // LSR zero page 0x46
    myasm = addopcode(&opmap, 0x46, "0x46 lsr $");
    opmap[0x46]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        lsrmem(cpu, mem, zeropage(cpu, mem));

        cpu->pc += 2;
        return 5;
    };

    // PHA 0x48
    myasm = addopcode(&opmap, 0x48, "0x48 pha");
    opmap[0x48]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s\n", myasm);
        push(cpu, mem, cpu->a);

        cpu->pc += 1;
        return 3;
    };

    // EOR immediate 0x49
    myasm = addopcode(&opmap, 0x49, "0x49 eor #$");
    opmap[0x49]->f = [cpu, mem, myasm]() {
        uint8_t val = mem->readmem(cpu->pc + 1);
        if (DEBUG) printf("\n%s%x\n", myasm, val);

        cpu->pc += 2;
        return 2;
    };

    // LSR accumulator 0x4a
    myasm = addopcode(&opmap, 0x4a, "0x4a lsr A");
    opmap[0x4a]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        lsra(cpu, &cpu->a);

        cpu->pc += 1;
        return 2;
    };

    // EOR absolute 0x4d (3, 4)
    myasm = addopcode(&opmap, 0x4d, "0x4d eor $");
    opmap[0x4d]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        EOR(cpu, mem->readmem(addr));

        cpu->pc += 3;
        return 4;
    };

    // LSR absolute 0x4e (3,6)
    myasm = addopcode(&opmap, 0x4e, "0x4e lsr $");
    opmap[0x4e]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);
        lsrmem(cpu, mem, addr);

        cpu->pc += 3;
        return 6;
    };

    // EOR indirect,y 0x51
    myasm = addopcode(&opmap, 0x51, "0x51 eor ($");
    opmap[0x51]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        EOR(cpu, mem->readmem(indirectindexed(cpu, mem)));

        cpu->pc += 2;
        return 5;
        // todo: +1 if page crossed
    };


    // EOR zero page,x 0x55 (2, 4)
    myasm = addopcode(&opmap, 0x55, "0x55 eor $");
    opmap[0x55]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        EOR(cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // LSR zero page,x 0x56 (2,6)
    myasm = addopcode(&opmap, 0x56, "0x56 lsr $");
    opmap[0x56]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, mem->readmem(cpu->pc + 1));
        lsrmem(cpu, mem, zeropagex(cpu, mem));

        cpu->pc += 2;
        return 6;
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

    // EOR absolute,y 0x59
    myasm = addopcode(&opmap, 0x59, "0x59 eor $");
    opmap[0x59]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        EOR(cpu, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // EOR absolute,x 0x5d
    myasm = addopcode(&opmap, 0x5d, "0x5d eor $");
    opmap[0x5d]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        EOR(cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // LSR absolute,x 0x5e (3,7)
    myasm = addopcode(&opmap, 0x5e, "0x5e lsr $");
    opmap[0x5e]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        lsrmem(cpu, mem, absolutex(cpu, mem));

        cpu->pc += 3;
        return 7;
    };

    // ADC indexed indirect
    myasm = addopcode(&opmap, 0x61, "0x61 adc ($");
    opmap[0x61]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        // NOTE: this had been sending an address to ADC before adding readmem()
        adc(cpu, mem->readmem(indexedindirect(cpu, mem)));

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

    // PLA 0x68
    myasm = addopcode(&opmap, 0x68, "0x68 pla");
    opmap[0x68]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s\n", myasm);
        cpu->a = pop(cpu, mem);
        setzeroflag(cpu->a, cpu);
        setnegflag(cpu->a, cpu);

        cpu->pc += 1;
        return 4;
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

    // DEY 0x88
    myasm = addopcode(&opmap, 0x88, "0x88 dey");
    opmap[0x88]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        decreg(cpu, &cpu->y);

        cpu->pc += 1;
        return 2;
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

    // LDY immediate 0xa0
    myasm = addopcode(&opmap, 0xa0, "0xa0 ldy #$");
    opmap[0xa0]->f = [cpu, mem, myasm]() {
        uint8_t val = mem->readmem(cpu->pc+1);
        if(DEBUG) printf("\n%s%x\n", myasm, val);
        ld(&cpu->y, cpu, val);

        cpu->pc += 2;
        return 2;
    };

    // LDA
    // indexed indirect address mode - slightly convoluted
    // http://www.emulator101.com/6502-addressing-modes.html
    myasm = addopcode(&opmap, 0xa1, "0xa1 lda ($");
    opmap[0xa1]->f = [cpu, mem, myasm]() {
        // todo: the readmem method returns a uint8_t, should it return uint16_t?
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        ld(&cpu->a, cpu, mem->readmem(indexedindirect(cpu, mem)));

        cpu->pc += 2;
        return 6;
    };

    // LDX immediate 0xa2
    myasm = addopcode(&opmap, 0xa2, "0xa2 ldx #$");
    opmap[0xa2]->f = [cpu, mem, myasm]() {
        uint8_t val = mem->readmem(cpu->pc+1);
        if(DEBUG) printf("\n%s%x\n", myasm, val);
        ld(&cpu->x, cpu, val);

        cpu->pc += 2;
        return 2;
    };

    // LDY zero page
    myasm = addopcode(&opmap, 0xa4, "0xa4 ldy $");
    opmap[0xa4]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        // I think passing a uint8_t to a uint16_t should work without issue
        ld(&cpu->y, cpu, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
        return 3;
    };

    // LDA zero page
    myasm = addopcode(&opmap, 0xa5, "0xa5 lda $");
    opmap[0xa5]->f = [cpu, mem, myasm]() {
        // todo: UNTESTED
        if(DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        // I think passing a uint8_t to a uint16_t should work without issue
        ld(&cpu->a, cpu, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
        return 3;
    };

    // LDX zero page 0xa6
    myasm = addopcode(&opmap, 0xa6, "0xa6 ldx $");
    opmap[0xa6]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        // I think passing a uint8_t to a uint16_t should work without issue
        ld(&cpu->x, cpu, mem->readmem(zeropage(cpu, mem)));

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
        ld(&cpu->a, cpu, val);

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

    // LDY absolute 0xac
    myasm = addopcode(&opmap, 0xac, "0xac ldy $");
    opmap[0xac]->f = [cpu, mem, myasm]() {

        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        ld(&cpu->y, cpu, mem->readmem(addr));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);

        cpu->pc += 3;
        return 4;
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
        ld(&cpu->a, cpu, mem->readmem(addr));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);

        cpu->pc += 3;
        return 4;
    };

    // LDX absolute 0xae
    myasm = addopcode(&opmap, 0xae, "0xae ldx $");
    opmap[0xae]->f = [cpu, mem, myasm]() {

        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        ld(&cpu->x, cpu, mem->readmem(addr));
        if(DEBUG) printf("\n%s%x\n", myasm, addr);

        cpu->pc += 3;
        return 4;
    };

    // LDA indirect indexed
    myasm = addopcode(&opmap, 0xb1, "0xb1 lda ($");
    opmap[0xb1]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        ld(&cpu->a, cpu, mem->readmem(indirectindexed(cpu, mem)));

        cpu->pc += 2;
        return 5;
        // todo: +1 if page crossed
    };

    // LDY zero page,X 0xb4
    myasm = addopcode(&opmap, 0xb4, "0xb4 ldy $");
    opmap[0xb4]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        ld(&cpu->y, cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // LDA zero page,X
    myasm = addopcode(&opmap, 0xb5, "0xb5 lda $");
    opmap[0xb5]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        ld(&cpu->a, cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // LDX zero page,Y 0xb6
    myasm = addopcode(&opmap, 0xb6, "0xb6 ldx $");
    opmap[0xb6]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n",myasm,mem->readmem(cpu->pc+1));
        ld(&cpu->x, cpu, mem->readmem(zeropagey(cpu, mem)));

        cpu->pc += 2;
        return 4;
    };

    // CLV 0xb8
    myasm = addopcode(&opmap, 0xb8, "0xb8 clv");
    opmap[0xb8]->f = [cpu, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        // clear oVerflow flag
        cpu->p &= ~(1 << 1);

        cpu->pc += 1;
        return 2;
    };

    // LDA absolute,Y
    myasm = addopcode(&opmap, 0xb9, "0xb9 lda $");
    opmap[0xb9]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        ld(&cpu->a, cpu, mem->readmem(absolutey(cpu, mem)));

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

    // 0xbc LDY absolute,X
    myasm = addopcode(&opmap, 0xbc, "0xbc ldy $");
    opmap[0xbc]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        ld(&cpu->y, cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // 0xbd LDA absolute,X
    myasm = addopcode(&opmap, 0xbd, "0xbd lda $");
    opmap[0xbd]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        ld(&cpu->a, cpu, mem->readmem(absolutex(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // LDX absolute,Y 0xbe
    myasm = addopcode(&opmap, 0xbe, "0xbe ldx $");
    opmap[0xbe]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        ld(&cpu->x, cpu, mem->readmem(absolutey(cpu, mem)));

        cpu->pc += 3;
        return 4;
        // todo: +1 if page crossed
    };

    // CPY immediate 0xc0
    myasm = addopcode(&opmap, 0xc0, "0xc0 cpy #$");
    opmap[0xc0]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->y, cpu->pc+1);

        cpu->pc += 2;
        return 2;
    };

    // CMP indirect,x 0xc1
    myasm = addopcode(&opmap, 0xc1, "0xc1 cmp ($");
    opmap[0xc1]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->a, indexedindirect(cpu, mem));

        cpu->pc += 2;
        return 6;
    };

    // CPY zero page 0xc4
    myasm = addopcode(&opmap, 0xc4, "0xc4 cpy $");
    opmap[0xc4]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->y, zeropage(cpu, mem));

        cpu->pc += 2;
        return 3;
    };


    // CMP zero page 0xc5
    myasm = addopcode(&opmap, 0xc5, "0xc5 cmp $");
    opmap[0xc5]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->a, zeropage(cpu, mem));

        cpu->pc += 2;
        return 3;
    };

    // DEC zero page 0xc6
    // you know, I'm having a thought here... this all could probably be done with macros pretty well, but that probably has it's own problems
    myasm = addopcode(&opmap, 0xc6, "0xc6 dec $");
    opmap[0xc6]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc + 1));
        decmem(cpu, mem, zeropage(cpu, mem));

        cpu->pc += 2;
        return 5;
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

    // CMP immediate 0xc9
    myasm = addopcode(&opmap, 0xc9, "0xc9 cmp #$");
    opmap[0xc9]->f = [cpu, mem, myasm]() {
        //uint8_t val = mem->readmem(cpu->pc+1);
        // NOTE: we reference immediate addressing differently between CMP and ADC
        // this is something to be aware of, todo: potential bug source elsewhere
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->a, cpu->pc+1);

        cpu->pc += 2;
        return 2;
    };

    // DEX 0xca
    myasm = addopcode(&opmap, 0xca, "0xca dex");
    opmap[0xca]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s\n", myasm);
        decreg(cpu, &cpu->x);

        cpu->pc += 1;
        return 2;
    };

    // CPY absolute 0xcc
    myasm = addopcode(&opmap, 0xcc, "0xcc cpy $");
    opmap[0xcc]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2));
        if (DEBUG) printf("\n%s%x\n", myasm, addr);
        compare(cpu, mem, cpu->y, addr);

        cpu->pc += 3;
        return 4;
    };

    // CMP absolute 0xcd
    myasm = addopcode(&opmap, 0xcd, "0xcd cmp $");
    opmap[0xcd]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2));
        if (DEBUG) printf("\n%s%x\n", myasm, addr);
        compare(cpu, mem, cpu->a, addr);

        cpu->pc += 3;
        return 4;
    };

    // DEC absolute 0xce (3, 6)
    myasm = addopcode(&opmap, 0xce, "0xce dec $");
    opmap[0xce]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2));
        if (DEBUG) printf("\n%s%x\n", myasm, addr);
        decmem(cpu, mem, addr);

        cpu->pc += 3;
        return 6;
    };

    // CMP indirect,y 0xd1
    myasm = addopcode(&opmap, 0xd1, "0xd1 cmp ($");
    opmap[0xd1]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->a, indirectindexed(cpu, mem));

        cpu->pc += 2;
        return 5;
        // todo: +1 if page crossed
    };

    // CMP zero page,x 0xd5
    myasm = addopcode(&opmap, 0xd5, "0xd5 cmp $");
    opmap[0xd5]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->a, zeropagex(cpu, mem));

        cpu->pc += 2;
        return 4;
    };

    // DEC zero page,x (2, 6) 0xd6
    myasm = addopcode(&opmap, 0xd6, "0xd6 dec $");
    opmap[0xd6]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, mem->readmem(cpu->pc+1));
        decmem(cpu, mem, zeropagex(cpu, mem));

        cpu->pc += 2;
        return 6;
    };

    // CLD 0xd8
    // clear decimal mode flag
    myasm = addopcode(&opmap, 0xd8, "0xd8 cld");
    opmap[0xd8]->f = [cpu, myasm] () {
        if (DEBUG) printf("\n%s\n", myasm);
        cpu->p &= ~(1 << 3);

        cpu->pc += 1;
        return 2;
    };

    // CMP absolute,y 0xd9
    myasm = addopcode(&opmap, 0xd9, "0xd9 cmp $");
    opmap[0xd9]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        compare(cpu, mem, cpu->a, absolutey(cpu, mem));

        cpu->pc += 3;
        return 4;   // todo: +1 if page crossed
    };

    // CMP absolute,x 0xdd
    myasm = addopcode(&opmap, 0xdd, "0xdd cmp $");
    opmap[0xdd]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        compare(cpu, mem, cpu->a, absolutex(cpu, mem));

        cpu->pc += 3;
        return 4;   // todo: +1 if page crossed
    };

    // DEC absolute,x 0xde (3, 7)
    myasm = addopcode(&opmap, 0xde, "0xde dec $");
    opmap[0xde]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        decmem(cpu, mem, absolutex(cpu, mem));

        cpu->pc += 3;
        return 7;
    };

    // CPX immediate 0xe0
    myasm = addopcode(&opmap, 0xe0, "0xe0 cpx #$");
    opmap[0xe0]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->x, cpu->pc+1);

        cpu->pc += 2;
        return 2;
    };

    // CPX zero page 0xe4
    myasm = addopcode(&opmap, 0xe4, "0xe4 cpx $");
    opmap[0xe4]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        compare(cpu, mem, cpu->x, zeropage(cpu, mem));

        cpu->pc += 2;
        return 3;
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

    // INX implied addressing mode 0xe8
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

    // NOP 0xea
    myasm = addopcode(&opmap, 0xea, "0xea nop");
    opmap[0xea]->f = [cpu, myasm] () {
        if(DEBUG) printf("\n%s\n", myasm);

        cpu->pc += 1;
        return 2;
    };

    // CPX absolute 0xec
    myasm = addopcode(&opmap, 0xec, "0xec cpx $");
    opmap[0xec]->f = [cpu, mem, myasm]() {
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2));
        if (DEBUG) printf("\n%s%x\n", myasm, addr);
        compare(cpu, mem, cpu->x, addr);


        cpu->pc += 3;
        return 4;
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

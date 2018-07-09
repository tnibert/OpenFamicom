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
    std::map<uint8_t, std::shared_ptr<opcode> > opmap;
    const char * myasm;

    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(uint8_t i = 0x0; i < 0xff; i++)
    {
        addopcode(&opmap, i, "test");
        opmap[i]->f = [i]() { printf("%x-", i); };
    }

    // todo: unit test all opcodes
    // http://www.emulator101.com/reference/6502-reference.html

    // BRK
    myasm = addopcode(&opmap, 0x00, "0x00 BRK");
    opmap[0x00]->f = [cpu, myasm]() {
        printf("\n%s\n", myasm);

        // todo: this actually does something important, implement

        cpu->pc += 1;
    };

    // LDA
    // indexed indirect address mode - slightly convoluted
    // http://www.emulator101.com/6502-addressing-modes.html
    myasm = addopcode(&opmap, 0xa1, "0xa1 lda ($");
    opmap[0xa1]->f = [cpu, mem, myasm]() {
        // todo: the readmem method returns a uint8_t, should it return uint16_t?
        printf("\n%s%x,X)\n", myasm, mem->readmem(cpu->pc+1));
        lda(cpu, mem->readmem(indexedindirect(cpu, mem)));

        cpu->pc += 2;
    };

    // LDA zero page
    myasm = addopcode(&opmap, 0xa5, "0xa5 lda $");
    opmap[0xa5]->f = [cpu, mem, myasm]() {
        // todo: UNTESTED
        printf("\n%s%x\n", myasm, mem->readmem(cpu->pc+1));
        // I think passing a uint8_t to a uint16_t should work without issue
        lda(cpu, mem->readmem(zeropage(cpu, mem)));

        cpu->pc += 2;
    };

    // LDA immediate
    myasm = addopcode(&opmap, 0xa9, "0xa9 lda #$");
    opmap[0xa9]->f = [cpu, mem, myasm]() {
        uint8_t val = mem->readmem(cpu->pc+1);
        printf("\n%s%x\n", myasm, val);
        lda(cpu, val);

        cpu->pc += 2;
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
        printf("\n%s%x\n", myasm, addr);

        cpu->pc += 3;
    };

    // LDA indirect indexed
    myasm = addopcode(&opmap, 0xb1, "0xb1 lda ($");
    opmap[0xb1]->f = [cpu, mem, myasm]() {
        printf("\n%s%x),Y\n", myasm, mem->readmem(cpu->pc+1));
        lda(cpu, mem->readmem(indirectindexed(cpu, mem)));

        cpu->pc += 2;
    };

    // LDA zero page,X
    myasm = addopcode(&opmap, 0xb5, "0xb5 lda $");
    opmap[0xb5]->f = [cpu, mem, myasm]() {
        printf("\n%s%x,X\n",myasm,mem->readmem(cpu->pc+1));
        lda(cpu, mem->readmem(zeropagex(cpu, mem)));

        cpu->pc += 2;
    };

    // LDA absolute,Y
    myasm = addopcode(&opmap, 0xb9, "0xb9 lda $");
    opmap[0xb9]->f = [cpu, mem, myasm]() {
        if(DEBUG) printf("\n%s%x,Y\n", myasm, revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2)));
        lda(cpu, mem->readmem(absolutey(cpu, mem)));
        cpu->pc += 3;
    };

    // 0xbd LDA absolute,X
    myasm = addopcode(&opmap, 0xbd, "0xbd lda $");
    opmap[0xbd]->f = [cpu, mem, myasm]() {
        if (DEBUG) printf("\n%s%x,X\n", myasm, revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
        lda(cpu, mem->readmem(absolutex(cpu, mem)));
        cpu->pc += 3;
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    myasm = addopcode(&opmap, 0xff, "DNE");
    opmap[0xff]->f = []() { printf("\n0xff DNE\n"); };

    return opmap;
}
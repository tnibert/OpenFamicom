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
    myasm = addopcode(&opmap, 0x00, "BRK");
    opmap[0x00]->f = [cpu, myasm]() {
        printf("\n%s\n", myasm);
        cpu->pc += 1;
    };

    // LDA $0314 constant
    // absolute address mode
    //delete &opmap[0xad];
    myasm = addopcode(&opmap, 0xad, "lda $");
    opmap[0xad]->f = [cpu, mem, myasm]() {
        /*
         * ok, so this is absolute addressing, 3 byte instruction
         * the two bytes after 0xad specify an exact addr to get data to put in A from
         * need to reverse for little endian
         */

        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        lda(cpu, mem->readmem(addr));
        printf("\n%s%x\n", myasm, addr);

        cpu->pc += 2;
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    myasm = addopcode(&opmap, 0xff, "DNE");
    opmap[0xff]->f = []() { printf("\n0xff DNE\n"); };

    return opmap;
}


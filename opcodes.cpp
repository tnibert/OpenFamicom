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
    std::shared_ptr<opcode> mycode;
    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(uint8_t i = 0x0; i < 0xff; i++)
    {
        addopcode(&opmap, i, "test");
        opmap[i]->f = [i]() { printf("%x-", i); };
    }

    // todo: unit test all opcodes

    // LDA $0314 constant
    // absolute address mode
    //delete &opmap[0xad];
    const char * myasm = addopcode(&opmap, 0xad, "lda $");
    opmap[0xad]->f = [cpu, mem, myasm]() {
        /*
         * ok, so this is absolute addressing, 3 byte instruction
         * the two bytes after 0xad specify an exact addr to get data to put in A from
         * need to reverse for little endian
         */

        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        cpu->a = mem->readmem(addr);
        printf("\n%s%x\n-", myasm, addr);

        // set flags, these are common to all LDA instructions I think
        // to set bits we use bitwise OR
        // todo: verify that flag implementation is correct
        /*
         * LDA affects the contents of the accumulator, does not affect
            the carry or overflow flags; sets the zero flag if the accumulator
            is zero as a result of the LDA, otherwise resets the zero flag;
            sets the negative flag if bit 7 of the accumulator is a 1, other­
            wise resets the negative flag.
            - is that bit 7 starting at 0 or 1?
         */
        // zero flag
        if(!cpu->a)
        {
            // set 0 flag
            // to do this we OR because Y OR 1 == 1, and Y OR 0 == 0
            cpu->p |= 0b00000010;
        }
        else
        {
            // reset 0 flag off
            // to do this we AND because Y AND 1 == Y
            cpu->p &= 0b11111101;
        }

        // negative flag
        // I'm going to assume we are setting bit 7 based on a start bit of 0
        // that seems to make most sense for a sign bit
        if(cpu->a & 0b10000000)           // on
        {
            cpu->p |= 0b10000000;
        }
        else            // off
        {
            cpu->p &= 0b01111111;
        }

        cpu->pc += 2;
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    mycode = std::shared_ptr<opcode>(new opcode(0xff, "test"));
    opmap[0xff] = mycode;
    opmap[0xff]->f = [mycode]() { printf("%x-", mycode->code); };

    return opmap;
}


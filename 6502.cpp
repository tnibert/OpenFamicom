#include "system.h"

// we will put this into the Famicom class
// put memory pointer in Famicom class
int Famicom::emulate6502op(unsigned char * buf)
{
    uint8_t opcode = buf[cpu->pc];      // this will need to be read into memory

    opmap[opcode]->f();

    cpu->pc++;          // next instruction

    return 0;
}

uint16_t Famicom::getpc()
{
    return cpu->pc;
}
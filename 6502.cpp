#include "system.h"

// we will put this into the Famicom class
// put memory pointer in Famicom class
int Famicom::emulate6502op()
{
    uint8_t opcode = memory[cpu->pc];

    opmap[opcode]->f();

    cpu->pc++;          // next instruction

    return 0;
}

uint16_t Famicom::getpc()
{
    return cpu->pc;
}
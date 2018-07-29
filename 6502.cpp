#include "system.h"

// we will put this into the Famicom class
// put memory pointer in Famicom class
int Famicom::emulate6502op()
{
    uint8_t opcode = memory->readmem(cpu->pc);

    opmap[opcode]->f();

    // we won't increment the pc here because that will be done in the opcode function

    return 0;
}

uint16_t Famicom::getpc()
{
    return cpu->pc;
}
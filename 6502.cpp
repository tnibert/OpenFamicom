#include "system.h"

// we will put this into the Famicom class
// put memory pointer in Famicom class
int Famicom::emulate6502op()
{
    // increment program counter for instruction read
    cpu->pc++;
    uint8_t opcode = memory->readmem(cpu->pc);

    int cycles = decoder->decode_and_execute(opcode);

    // synchronize ppu
    //for(int i = 0; i < cycles; i++)
    //{
        // execute ppu cycles
    //}

    return 0;
}

uint16_t Famicom::getpc()
{
    return cpu->pc;
}


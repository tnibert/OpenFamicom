#include "system.h"

// we will put this into the Famicom class
// put memory pointer in Famicom class
int Famicom::emulate6502op()
{
    uint8_t opcode = memory->readmem(cpu->pc);

    // OK cycles is not equivalent to incrementation of the pc
    // this is wrong
    // todo: figure out difference between cycles and pc incrementation and implement correctly
    int cycles = opmap[opcode]->f();

    cpu->pc += cycles;

    // synchronize ppu
    for(int i = 0; i < cycles; i++)
    {
        // execute ppu cycles
    }

    return 0;
}

uint16_t Famicom::getpc()
{
    return cpu->pc;
}
#include "system.h"

// we will put this into the Famicom class
// put memory pointer in Famicom class
int emulate6502op(cpustate * state, std::map<uint8_t, std::shared_ptr<opcode> > opmap)
{
    uint8_t opcode = &state->memory[state->pc];

    opmap[opcode]->f();
}

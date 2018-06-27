#include "system.h"

int emulate6502op(cpustate * state, std::map<unsigned char *, std::shared_ptr<opcode> > opmap)
{
    unsigned char *opcode = &state->memory[state->pc];

    opmap[*opcode]->f();
}

#ifndef OPENNES_DECODER_H
#define OPENNES_DECODER_H

#include "system.h"

// forward declaration
class Memory;
struct cpustate;

class InstructionDecoder
{
    cpustate * cpu;
    Memory * mem;
    public:
        InstructionDecoder(cpustate *, Memory *);
        int decode_and_execute(uint8_t);
        void decode_aaabbbcc(uint8_t);
        bool decode_branch(uint8_t);
};


#endif //OPENNES_DECODER_H

//
// Created by tim on 10/13/19.
//

#ifndef OPENNES_DECODER_H
#define OPENNES_DECODER_H


class InstructionDecoder
{
    cpustate * cpu;
    std::map<uint8_t, std::shared_ptr<opcode> > opmap;
};


#endif //OPENNES_DECODER_H

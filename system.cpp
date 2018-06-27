#include "system.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <functional>
/*#include <memory>
#include <thread>
#include <chrono>
#include <mutex>*/

Famicom::Famicom(void) {
    using namespace std;
    cout << "Famicom object is being created" << endl;
    // initialize cpu state
    cpu = (cpustate *)calloc(1, sizeof(cpustate));
    //cpu->pc = ?;

    std::map<uint8_t, std::shared_ptr<opcode> > opmap = create_opcode_map(cpu);
    opmap[0x1a]->f();
}

opcode::opcode(uint8_t ocode)
{
    code = ocode;
}

void opcode::printcode()
{
    printf("%x\n", code);
}

std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate * cpu)
{
    std::map<uint8_t, std::shared_ptr<opcode> > opmap;

    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(uint8_t i = 0x0; i < 0xff; i++)
    {
        std::shared_ptr<opcode> mycode(new opcode(i));
        opmap[i] = mycode;
        //opmap[i]->printcode();
        opmap[i]->f = [cpu, mycode]() { printf("%x\n", mycode->code); };
    }

    return opmap;
}

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

    std::map<unsigned char *, std::shared_ptr<opcode> > opmap = create_opcode_map(cpu);
    //opmap[0x1a]->f();
}

opcode::opcode(unsigned char * ocode)
{
    code = ocode;
}

void opcode::printcode()
{
    printf("%x\n", code);
}

std::map<unsigned char *, std::shared_ptr<opcode> > create_opcode_map(cpustate * cpu)
{
    std::map<unsigned char *, std::shared_ptr<opcode> > opmap;

    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(unsigned char i = 0x00; i < 0xff; i++)
    {
        std::shared_ptr<opcode> mycode(new opcode(&i));
        opmap[&i] = mycode;
        opmap[&i]->printcode();
        opmap[&i]->f = [cpu, mycode]() { std::cout << mycode << std::endl; };
    }

    return opmap;
}

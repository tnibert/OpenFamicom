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

    std::map<int, std::shared_ptr<opcode> > opmap = create_opcode_map(cpu);
    opmap[0x1a]->f();
}

opcode::opcode(int ocode)
{
    code = ocode;
}

void opcode::printcode()
{
    printf("%x\n", code);
}

std::map<int, std::shared_ptr<opcode> > create_opcode_map(cpustate * cpu)
{
    std::map<int, std::shared_ptr<opcode> > opmap;

    // now we need to do this for all opcodes
    std::function<void ()> f = []() { std::cout << "Test map created" << std::endl; };
    std::shared_ptr<opcode> mycode(new opcode(0x1a));
    opmap[0x1a] = mycode;
    opmap[0x1a]->printcode();
    opmap[0x1a]->f = f;

    return opmap;
}

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

Famicom::Famicom(unsigned char * rom) {
    using namespace std;
    cout << "Famicom object is being created" << endl;

    // initialize cpu state
    cpu = (cpustate *)calloc(1, sizeof(cpustate));
    cpu->pc = PRGROMSTART;                         // may not start at 0
    // todo: verify all initial values
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->p = 0x34;
    cpu->sp = 0xfd;

    // initialize cartridge
    cart = new Cartridge(rom);

    // create memory map
    memory = new uint8_t[0xffff];         // 2 kB of onboard work RAM, cartridges can add, 2kb = 0x800

    for(int i = 0; i < cart->prgromsize; i++)
    {
        memory[0x4020+i] = cart->prgrom[i];                                     // this copies the data from cart->prgrom to memory[0x4020], not the reference
    }
    //printf("prgrom start in memory: %x, cartridge prgrom start: %x\n", memory[0x4020], cart->prgrom);
    opmap = create_opcode_map(cpu, &memory[0x4020]);
}

opcode::opcode(uint8_t ocode, const char * ocodestr)
{
    code = ocode;
    opcodestr = ocodestr;
}

void opcode::printcode()
{
    printf("%x\n", code);
}

std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate * cpu, uint8_t * mem)
{
    std::map<uint8_t, std::shared_ptr<opcode> > opmap;
    std::shared_ptr<opcode> mycode;
    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(uint8_t i = 0x0; i < 0xff; i++)
    {
        mycode = std::shared_ptr<opcode>(new opcode(i, "test"));
        opmap[i] = mycode;
        opmap[i]->f = [mycode]() { printf("%x-", mycode->code); };
    }

    // todo: unit test all opcodes

    // todo: create a function for the following assignment procedure
    // LDA $0314 constant
    // absolute address mode
    //delete &opmap[0xad];
    mycode = std::shared_ptr<opcode>(new opcode(0xad, "lda $"));
    opmap[0xad] = mycode;
    opmap[0xad]->f = [cpu, mem, mycode]() {
        /*
         * I don't think this is quite correct, but we'll work with it
         * need to do a bit shift?
         * definitely not quite correct, we seg fault as soon as we hit this
         * probably because we haven't assigned anything to nes memory...
         */
        cpu->pc += 1;
        cpu->a = mem[cpu->pc];
        printf("%s%x-", mycode->opcodestr, cpu->a);
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    mycode = std::shared_ptr<opcode>(new opcode(0xff, "test"));
    opmap[0xff] = mycode;
    opmap[0xff]->f = [mycode]() { printf("%x-", mycode->code); };

    return opmap;
}

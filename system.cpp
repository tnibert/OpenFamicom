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
    memory = new Memory();         // 2 kB of onboard work RAM, cartridges can add, 2kb = 0x800

    memory->loadprgrom(cart);
    //printf("prgrom start in memory: %x, cartridge prgrom start: %x\n", memory[0x4020], cart->prgrom);
    opmap = create_opcode_map(cpu, memory);
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

void addopcode(std::map<uint8_t, std::shared_ptr<opcode> > * curmap, uint8_t code, const char * inststr)
{
    //
    (*curmap)[code] = std::shared_ptr<opcode>(new opcode(code, inststr));
}

uint16_t revlendianbytes() {};

std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate * cpu, Memory * mem)
{
    std::map<uint8_t, std::shared_ptr<opcode> > opmap;
    std::shared_ptr<opcode> mycode;
    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(uint8_t i = 0x0; i < 0xff; i++)
    {
        addopcode(&opmap, i, "test");
        opmap[i]->f = [i]() { printf("%x-", i); };
    }

    // todo: unit test all opcodes

    // todo: create a function for the following assignment procedure
    // LDA $0314 constant
    // absolute address mode
    //delete &opmap[0xad];
    const char * myasm = "lda $";
    addopcode(&opmap, 0xad, "lda $");
    opmap[0xad]->f = [cpu, mem, myasm]() {
        /*
         * ok, so this is absolute addressing, 3 byte instruction
         * the two bytes after 0xad specify an exact addr to get data to put in A from
         * need to reverse for little endian
         */
        //uint16_t

        cpu->a = mem->readmem(cpu->pc);
        printf("%s%x-", myasm, cpu->a);
        cpu->pc += 2;
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    mycode = std::shared_ptr<opcode>(new opcode(0xff, "test"));
    opmap[0xff] = mycode;
    opmap[0xff]->f = [mycode]() { printf("%x-", mycode->code); };

    return opmap;
}

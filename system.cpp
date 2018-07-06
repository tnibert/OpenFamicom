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

const char * addopcode(std::map<uint8_t, std::shared_ptr<opcode> > * curmap, uint8_t code, const char * inststr)
{
    /*
     * creates a new opcode in the opcode map
     * returns the string passed in
     * NOTE: I'm not sure if this method will leak the old contents of memory or not if already assigned...
     * cursory research suggests no
     */
    (*curmap)[code] = std::shared_ptr<opcode>(new opcode(code, inststr));
    return inststr;
}

uint16_t revlendianbytes(uint8_t lsig, uint8_t msig)
{
    /*
     * lsig == lest significant
     * msig == most significant
     * In the rom, the least significant bit comes first
     */
    uint16_t offset = (msig<<8) | (lsig);
    return offset;
}

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

    // LDA $0314 constant
    // absolute address mode
    //delete &opmap[0xad];
    const char * myasm = addopcode(&opmap, 0xad, "lda $");
    opmap[0xad]->f = [cpu, mem, myasm]() {
        /*
         * ok, so this is absolute addressing, 3 byte instruction
         * the two bytes after 0xad specify an exact addr to get data to put in A from
         * need to reverse for little endian
         * todo: implement flags
         */
        printf("\n\n%x",mem->readmem(cpu->pc+1));
        printf("%x",mem->readmem(cpu->pc+2));
        uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1),mem->readmem(cpu->pc+2));
        printf("\n\n%x\n\n", addr);
        cpu->a = mem->readmem(addr);
        printf("%s%x-", myasm, addr);
        cpu->pc += 2;
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    mycode = std::shared_ptr<opcode>(new opcode(0xff, "test"));
    opmap[0xff] = mycode;
    opmap[0xff]->f = [mycode]() { printf("%x-", mycode->code); };

    return opmap;
}

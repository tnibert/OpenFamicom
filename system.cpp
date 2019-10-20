#include "system.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <functional>

Famicom::Famicom(unsigned char * rom) {
    using namespace std;
    cout << "Famicom object is being created" << endl;

    cpu = initcpu();

    // initialize cartridge
    cart = new Cartridge(rom);

    // create memory map
    memory = new Memory();         // 2 kB of onboard work RAM, cartridges can add, 2kb = 0x800

    memory->loadprgrom(cart);
    //printf("prgrom start in memory: %x, cartridge prgrom start: %x\n", memory[0x4020], cart->prgrom);
    //opmap = create_opcode_map(cpu, memory);
    decoder = new InstructionDecoder(cpu, memory);
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

cpustate * initcpu()
{
    // initialize cpu state
    cpustate * cpu = (cpustate *)calloc(1, sizeof(cpustate));
    cpu->pc = PRGROMSTART;
    // todo: verify all initial values
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->p = 0x34;      // processor status flags
    cpu->sp = 0xfd;

    return cpu;
}
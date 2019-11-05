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


/**
 * 6502 is little endian.  Therefore we must swap bytes to resolve final addresses
 * e.g. in memory 0x28 followed in 0x40 resolves the the 16 bit address 0x4028
 * @param lsig: least significant byte
 * @param msig: most significant byte
 * @return 16 bit value
 */
uint16_t revlendianbytes(uint8_t lsig, uint8_t msig)
{
    uint16_t offset = (msig<<8) | (lsig);
    return offset;
}

/**
 * Convert a two's comp value to regular signed number
 * @param in
 * @return
 */
int8_t decode_twos_comp(uint8_t in)
{
    // if not two's comp, return the original number
    if((in & 0b10000000) == 0b0)
    {
        return in;
    }
    else
    {
        uint8_t minusone = in - 0b1;
        return -(~minusone);
    }
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
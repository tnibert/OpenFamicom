#ifndef system_H
#define system_H

#include <stdint.h>
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <assert.h>

#define PPUREGSTART 0x2000
#define APUIOREGSTART 0x4000
#define CPUTESTMODESTART 0x4018
#define PRGROMSTART 0x4020

// class for instructions
class opcode
{
    public:
        opcode(uint8_t, const char *);
        void printcode();
        const char * opcodestr;
        uint8_t code;                           // may want to write an accessor
        std::function<void ()> f;               // this doesn't seem to have access to the variables for the opcode object
};

typedef struct cpustate {
    // litte endian
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t p;
    uint8_t sp;
    uint16_t pc;
} cpustate;

class Cartridge {
    /* class to manage .nes files */
    // https://wiki.nesdev.com/w/index.php/INES
    public:
        uint8_t header[16];
        uint8_t * trainer;
        uint8_t * prgrom;
        int prgromsize;
        uint8_t * chrrom;
        uint8_t * PlayChoiceINSTROM;
        uint8_t * PlayChoicePROM;
        uint8_t * prgram;
        uint8_t flags6;
        uint8_t flags7;
        uint8_t flags9;
        uint8_t flags10;
        Cartridge(unsigned char *);
};

class Memory
{
    uint8_t * internalram;
    uint8_t * ppureg;
    uint8_t * apuioreg;
    uint8_t * cputestmode;
    uint8_t * prg;
    public:
        Memory();
        void loadprgrom(Cartridge *);
        // the following are for reading and writing memory, uint16_t is the address
        void writemem(uint16_t, uint8_t);
        uint8_t readmem(uint16_t);
};

class Famicom {
    cpustate * cpu;
    Memory * memory;
    public:
        Famicom(unsigned char *);
        std::map<uint8_t, std::shared_ptr<opcode> > opmap;      // can be private
        int emulate6502op();
        uint16_t getpc();
        Cartridge * cart;
};

uint16_t revlendianbytes(uint8_t, uint8_t);

std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate *, Memory *);

// common asm operations
void lda(cpustate *, uint8_t);

// addressing modes
uint8_t indexedindirect(cpustate *, Memory *);

uint8_t zeropage(cpustate *, Memory *);

uint8_t indirectindexed(cpustate *, Memory *);

#endif

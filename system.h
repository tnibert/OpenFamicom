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

// class for instructions
class opcode
{
    public:
        opcode(uint8_t, const char [4]);
        void printcode();
        std::string opcodestr;
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
    uint8_t header[16];
    uint8_t * trainer;
    uint8_t * prgrom;
    uint8_t * chrrom;
    uint8_t * PlayChoiceINSTROM;
    uint8_t * PlayChoicePROM;
    uint8_t * prgram;
    uint8_t flags6;
    uint8_t flags7;
    uint8_t flags9;
    uint8_t flags10;
public:
    Cartridge(unsigned char *);
};

// this will all definitely need to be refactored e.g. put emulation function in class, etc

class Famicom {
    cpustate * cpu;
    uint8_t * memory;
    public:
        Famicom(unsigned char *);
        std::map<uint8_t, std::shared_ptr<opcode> > opmap;      // can be private
        int emulate6502op(unsigned char *);
        uint16_t getpc();
        Cartridge * cart;
};



std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate *, uint8_t *);

#endif

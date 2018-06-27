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

// 6502 condition codes?
typedef struct ConditionCodes {

} ConditionCodes;

typedef struct cpustate {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t p;
    uint8_t sp;
    uint16_t pc;
} cpustate;


// this will all definitely need to be refactored e.g. put emulation function in class, etc

class Famicom {
    //cpustate * cpu;
    public:
        Famicom(void);
        cpustate * cpu;
        std::map<uint8_t, std::shared_ptr<opcode> > opmap;
};

// class for instructions
class opcode
{
    //uint8_t code;       // may need to be uint8_t
    std::string opcodestr;
    public:
        uint8_t code;               // may want to write an accessor
        std::function<void ()> f;
        opcode(uint8_t ocode);
        void printcode();
};

std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate *);

int emulate6502op(cpustate * state, std::map<uint8_t, std::shared_ptr<opcode> >)

#endif

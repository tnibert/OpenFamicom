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

class Famicom {
    cpustate * cpu;
    public:
        Famicom(void);
};

// class for instructions
class opcode
{
    int code;       // may need to be uint8_t
    std::string opcodestr;
    public:
        std::function<void ()> f;
        opcode(int);
        void printcode();
};

std::map<int, std::shared_ptr<opcode> > create_opcode_map(cpustate *);

#endif

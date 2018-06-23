#include <stdint.h>

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
};

// create an abstract class for instructions
class opcode
{
    unsigned char * opcode;
    char * opcodestr;
    virtual void action()=0;
};

// create a hash table for instructions


int main()
{
    return 0;
}

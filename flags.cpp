#include "system.h"

bool setzeroflag(cpustate * cpu)
{
    // if accumulator is 0, zero flag is set
    if(!cpu->a)
    {
        cpu->p |= (1 << 6);
        return true;
    }
    else
    {
        cpu->p &= ~(1 << 6);
        return false;
    }
}

bool setnegflag(cpustate * cpu)
{
    // if bit 7 of accumulator is on, negative flag is set
    if(cpu->a & 0x80)
    {
        cpu->p |= 1;
        return true;
    }
    else
    {
        cpu->p &= ~1;
        return false;
    }
}
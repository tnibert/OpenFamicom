#include "system.h"

/*
 * setter functions return true or false based on whether flag was set or not
 */

bool setzeroflag(uint8_t compval, cpustate * cpu)
{
    // if compval is 0, zero flag is set
    if(compval == 0)
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

bool setnegflag(uint8_t compval, cpustate * cpu)
{
    // if bit 7 of compval is on, negative flag is set
    if(compval & 0x80)
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
#include "system.h"

/*
 * setter functions return true or false based on whether flag was set or not
 *
 * order of flags:
 *      Bit No.    7   6   5   4   3   2   1   0
                   S   V       B   D   I   Z   C
        S = Sign = Negative
        V = oVerflow
        bit 5 is always set to one
        B = break flag
        D = decimal flag (not used in NES)
        I = interrupt
        Z = zero
        C = carry

  * https://wiki.nesdev.com/w/index.php/Status_flags
 */

bool setzeroflag(uint8_t compval, cpustate * cpu)
{
    // if compval is 0, zero flag is set
    if(compval == 0)
    {
        cpu->p |= (1 << 1);
        return true;
    }
    else
    {
        cpu->p &= ~(1 << 1);
        return false;
    }
}

bool setnegflag(uint8_t compval, cpustate * cpu)
{
    // if bit 7 of compval is on, negative flag is set
    if(compval & 0x80)
    {
        cpu->p |= (1 << 7);
        return true;
    }
    else
    {
        cpu->p &= ~(1 << 7);
        return false;
    }
}

bool getflag(cpustate * cpu, int bit)
{
    return (cpu->p & (1 << bit)) == (1 << bit);
}
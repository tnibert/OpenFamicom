#include "system.h"

/*
 * This file defines functions that have common code between opcodes
 * For example, lda has several opcodes for different addressing modes
 * but outside of the addressing modes, they basically do the same thing.
 * There is also instructions like lda, ldx, ldy - same concept, different registers
 *
 * Keep our code compact and manageable
 *
 * This will probably mostly be for handling flags
 * These functions will not increase the program counter as the instruction size may vary
 */

void lda(cpustate * cpu, uint8_t data)
{
    cpu->a = data;

    // set flags, these are common to all LDA instructions I think
    // to set bits we use bitwise OR
    // todo: verify that flag implementation is correct
    /*
     * LDA affects the contents of the accumulator, does not affect
        the carry or overflow flags; sets the zero flag if the accumulator
        is zero as a result of the LDA, otherwise resets the zero flag;
        sets the negative flag if bit 7 of the accumulator is a 1, other­
        wise resets the negative flag.
        - is that bit 7 starting at 0 or 1?
     */
    // zero flag
    if(!cpu->a)
    {
        // set 0 flag
        // to do this we OR because Y OR 1 == 1, and Y OR 0 == 0
        cpu->p |= 0b00000010;
    }
    else
    {
        // reset 0 flag off
        // to do this we AND because Y AND 1 == Y
        cpu->p &= 0b11111101;
    }

    // negative flag
    // I'm going to assume we are setting bit 7 based on a start bit of 0
    // that seems to make most sense for a sign bit
    if(cpu->a & 0b10000000)           // on
    {
        cpu->p |= 0b10000000;
    }
    else            // off
    {
        cpu->p &= 0b01111111;
    }
}
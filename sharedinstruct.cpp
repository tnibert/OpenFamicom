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

// todo: unit test all of this

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

void ldflags(cpustate * cpu)
{
    // confirm whether or not lda, ldx, and ldy set flags the same way
}

void sta(cpustate * cpu, Memory * mem, uint16_t addr)
{
    /*
     * This instruction transfers the contents of the accumulator to
        memory.
        The symbolic representation for this instruction is A -> M.
        This instruction affects none of the flags in the processor
        status register and does not affect the accumulator.
        It is a "Group One" instruction and has the following address­
        ing modes available to it:
        Absolute; Zero Page; Absolute,X; Absolute,Y; Zero Page,X;
        Indexed Indirect; and Indirect Indexed.
     */
    mem->writemem(addr, cpu->a);
}

void adc()
{
    /*
     * Group 1 instruction
     * NES processor doesn't use binary coded decimal, thank god
     * This instruction adds the value of memory and carry from the
        previous operation to the value of the accumulator and stores the
        result: in the accumulator.
        The symbolic representation for this instruction is
        A + M + C -> A.
        This instruction affects the accumulator; sets the carry flag
        when the sum of a binary add exceeds 255 or when the sum of a decimal
        add exceeds 99, otherwise carry is reset.
        The overflow flag is set
        when the sign or bit 7 is changed due to the result exceeding +127
        or -128, otherwise overflow is reset.
        The negative flag is set if
        the accumulator result contains bit 7 on, otherwise the negative
        flag is reset.
        The zero flag is set if the accumulator result is 0,
        otherwise the zero flag is reset.
     */
}
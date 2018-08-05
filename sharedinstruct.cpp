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
 *
 * If an instruction has only one opcode, it will be implemented in opcodes.cpp
 *
 * http://obelisk.me.uk/6502/reference.html
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

void stx(cpustate * cpu, Memory * mem, uint16_t addr)
{
    // transfers contents of x register to memory
    mem->writemem(addr, cpu->x);
}

void sty(cpustate * cpu, Memory * mem, uint16_t addr)
{
    // transfers contents of y register to memory
    mem->writemem(addr, cpu->y);
}

void adc(cpustate * cpu, uint8_t arg)
{
    // todo: I'm not hanging with the math here so I definitely need to unit test and get my head into it
    // remember the cardinal rule: whatever is not understood will be the cause of bugs
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

        implementation:
        from https://stackoverflow.com/a/29224684
        need to understand it...

        flags:
        CZ- - - VN
        V is oVerflow flag

        http://www.learncpp.com/cpp-tutorial/3-8a-bit-flags-and-bit-masks/
     */
    bool carry = cpu->p & (1 << 7);

    unsigned const sum = cpu->a + arg + carry;

    // carry flag test
    if(sum > 0xFF)
    {
        // set carry
        cpu->p |= (1 << 7);
    }
    else {
        // clear carry
        cpu->p &= ~(1 << 7);
    }

    // The overflow flag is set when the sign of the addends is the same and
    // differs from the sign of the sum
    // 0x80 is hex for 0b10000000
    // ^ is bitwise xor operator
    if(~(cpu->a ^ arg) & (cpu->a ^ sum) & 0x80)
    {
        // set overflow flag
        cpu->p |= (1 << 1);
    }
    else
    {
        // clear overflow flag
        cpu->p &= ~(1 << 1);
    }

    // I think the front bits will just fall off if we overflow the uint8_t??
    cpu->a = sum;

    setnegflag(cpu->a, cpu);
    setzeroflag(cpu->a, cpu);
}

void sbc(cpustate * cpu, uint8_t arg) { adc(cpu, ~arg); }

void AND(cpustate * cpu, uint8_t arg)
{
    /*
    AND is a "Group One" instruction having addressing modes of
    Immediate; Absolute; Zero Page; Absolute,X; Absolute,Y; Zero Page,X;
    Indexed Indirect; and Indirect Indexed.
     */
    cpu->a = cpu->a & arg;

    setzeroflag(cpu->a, cpu);
    setnegflag(cpu->a, cpu);
}

void OR()
{

}

void EOR()
{

}

// Arithmetic Shift Left
void asl()
{

}

// Logical Shift Right
void lsr()
{

}

// Rotate Right
void ror()
{

}


void inc(cpustate * cpu, Memory * mem, uint16_t finaladdr)
{
    /* Adds one to the value held at a specified memory location setting the zero and negative flags as appropriate.
     * sets zero and negative flags
        cpu - cpustate struct,
        mem - cpu memory class
        finaladdr - the address directly pointing to the value to increment (after addressing mode translation)
        todo: unit test
     */
    uint8_t val = mem->readmem(finaladdr);
    if(val == 255)
    val++;
    mem->writemem(finaladdr, val);

    // zero flag set or unset
    setzeroflag(val, cpu);

    // negative flag ser or unset
    setnegflag(val, cpu);
}
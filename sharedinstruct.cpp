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

void ld(uint8_t * reg, cpustate * cpu, uint8_t data)
{
    /*
     * reg: address of the register to load to e.g. &cpu->a
     * cpu: cpu state machine
     * data: data to load into the register
     */
    *reg = data;

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

    setzeroflag(*reg, cpu);
    setnegflag(*reg, cpu);
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

void ORA(cpustate * cpu, uint8_t arg)
{
    cpu->a = cpu->a | arg;

    setzeroflag(cpu->a, cpu);
    setnegflag(cpu->a, cpu);
}

void EOR(cpustate * cpu, uint8_t arg)
{
    cpu->a = cpu->a ^ arg;

    setzeroflag(cpu->a, cpu);
    setnegflag(cpu->a, cpu);
}

// Arithmetic Shift Left
uint8_t _asl(cpustate * cpu, uint8_t val)
{
    /*
     * uint8_t val: pointer to either the accumulator or a value read from memory
     *
     * flags (from MOS 6502 programming manual):
     *      The instruction does not affect the overflow bit, sets N equal to the
     *       result bit 7 (bit 6 in the input), sets Z flag if the result is equal to
     *       0, otherwise resets Z and stores the input bit 7 in the carry flag.
     */

    // set carry flag to bit 7 of input value
    cpu->p |= (val & 0x80);

    // bit shift
    val = val << 1;

    setzeroflag(val, cpu);
    setnegflag(val, cpu);

    return val;
}

// ASL for accumulator
void asla(cpustate * cpu, uint8_t * val)
{
    *val = _asl(cpu, *val);
}

// ASL for memory address
void aslmem(cpustate * cpu, Memory * mem, uint16_t addr)
{
    uint8_t val = mem->readmem(addr);

    val = _asl(cpu, val);

    mem->writemem(addr, val);
}

// Logical Shift Right
uint8_t _lsr(cpustate * cpu, uint8_t val)
{
    // set carry flag to bit 0 of input value
    if(val & 0x1)
    {
        cpu->p |= 0x80;
    }
    else
    {
        cpu->p &= ~0x80;
    }

    // bit shift
    val = val >> 1;

    setzeroflag(val, cpu);
    setnegflag(val, cpu);

    return val;
}

// LSR for accumulator
void lsra(cpustate * cpu, uint8_t * val)
{
    *val = _lsr(cpu, *val);
}

// LSR for memory address
void lsrmem(cpustate * cpu, Memory * mem, uint16_t addr)
{
    uint8_t val = mem->readmem(addr);

    val = _lsr(cpu, val);

    mem->writemem(addr, val);
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
    // removed check for val == 255, I think it will just roll around to zero again?
    val++;
    mem->writemem(finaladdr, val);

    // zero flag set or unset
    setzeroflag(val, cpu);

    // negative flag ser or unset
    setnegflag(val, cpu);
}

uint8_t _dec(cpustate * cpu, uint8_t val)
{
    /*
        Used for DEC, DEX, DEY
        Decrement and set flags
    */
    val--;

    setzeroflag(val, cpu);
    setnegflag(val, cpu);
    return val;
}

void decmem(cpustate * cpu, Memory * mem, uint16_t addr)
{
    // for DEC
    // uint16_t addr: final address of memory
    uint8_t val = mem->readmem(addr);
    val = _dec(cpu, val);
    mem->writemem(addr, val);
}

void decreg(cpustate * cpu, uint8_t * reg)
{
    // for DEX, DEY
    // call like: decreg(cpu, &cpu->x);
    // http://faculty.cs.niu.edu/~mcmahon/CS241/Notes/pass_by_address.html
    *reg = _dec(cpu, *reg);
}

void compare(cpustate * cpu, Memory * mem, uint8_t reg, uint16_t addr)
{
    /*
     * Used for CMP, CPX, and CPY
     * reg: the register to compare against
     * addr: the final memory address to retrieve the value from
     *
     * http://www.6502.org/tutorials/compare_instructions.html
     * The compare instructions subtract (without carry) an immediate value or the contents of a memory location from
     * the addressed register, but do not save the result in the register. The only indications of the results are the
     * states of the three status flags: Negative (N), Zero (Z), and Carry (C). The combination of these three flags
     * indicate whether the register contents are less than, equal to (the same as), or greater than the operand "data"
     * (the immediate value or contents of the addressed memory location.
     */
    uint8_t value = mem->readmem(addr);
    // we probably *should* use the result value to calculate the flags, but hey, power of abstraction
    uint8_t result = reg - value;

    /* from MOS 6502 programming manual:
    the carry flag is set when the value in
    memory is less than or equal to the accumulator, reset when it is
    greater than the accumulator*/
    if(reg >= value)
    {
        cpu->p |= (1 << 7);
    }
    else
    {
        cpu->p &= ~(1 << 7);
    }

    // if equal set zero flag
    if(reg == value)
    {
        cpu->p |= (1 << 6);
    }
    else
    {
        cpu->p &= ~(1 << 6);
    }

    // set negative flag if bit 7 of the result is set
    setnegflag(result, cpu);
}

/*
 * For push and pull, the 6502 is an EMPTY STACK
 * https://wiki.nesdev.com/w/index.php/Stack
 * todo: the addressing for our stack pointer might be wrong... verify
 * Can we initialize it to different locations at the start?
 * Do we need to handle stack overflow and underflow?
 */

void push(cpustate * cpu, Memory * mem, uint8_t val)
{
    /*
     * Push a value to the stack
     * write to stack pointer address, then decrement stack pointer by 1
     */
    mem->writemem(cpu->sp, val);
    cpu->sp--;
}

uint8_t pop(cpustate * cpu, Memory * mem)
{
    /*
     * pop a value from the stack
     * increment stack pointer by 1, then read from stack point address
     * storageloc: a point to the cpu register that we are storing the memory read to
     */
    cpu->sp++;
    return mem->readmem(cpu->sp);
}

void bit(cpustate * cpu, Memory * mem, uint16_t addr)
{
    /*
     * Compares a memory value with accumulator and sets zero flag
     * sets overflow and negative flags to bits 6 and 7 of memory value
     */
    uint8_t data = mem->readmem(addr);

    // if memory and accumulator equals zero, set zero flag
    setzeroflag(cpu->a & data, cpu);

    // overflow flag set to bit 6 of the memory value
    // we may want to incorporate this into its own function
    // todo: verify correctness of 0x40
    if(data & 0x40)
    {
        cpu->p |= 0x40;
    }
    else
    {
        cpu->p &= ~0x40;
    }

    // negative flag set to bit 7 of the memory value
    setnegflag(data, cpu);
}

#include "system.h"

/*
 * This file includes function for the various addressing modes used by the 6502
 * http://www.emulator101.com/6502-addressing-modes.html
 * http://www.obelisk.me.uk/6502/addressing.html
 * https://www.youtube.com/watch?v=kk0aW_p8EeE
 *
 * These functions will resolve the addressing mode to the final address
 * All functions in this file have the same inputs and outputs:
 * Take arguments of cpu and memory pointers
 * We assume that the pc is pointing at the current opcode
 * return resolution of final address of data
 *
 * We must return the final address because some instructions read and write from a memory location
 * with one addressing mode, but don't for another (e.g. accumulator, immediate)
 *
 * todo: double check these, unit test
 * todo: update cycle count for every memory access
 */

/**
 * immediate addressing is passing the value specified in code, a constant
 * Return the 16 bit address of the constant
 */
uint16_t immediate(cpustate * cpu)
 {
     cpu->pc++;
     return cpu->pc;
 }

 /**
  * Return 8 bit address from ROM
  * @param cpu
  * @param mem
  * @return
  */
uint8_t zeropage(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    uint8_t finaladdr = mem->readmem(cpu->pc);
    return finaladdr;
}

/**
 * Return full 16 bit address from ROM
 * @param cpu
 * @param mem
 * @return
 */
uint16_t absolute(cpustate * cpu, Memory * mem)
{
    cpu->pc+=2;
    return revlendianbytes(mem->readmem(cpu->pc-1), mem->readmem(cpu->pc));
}

/**
 * Return zero page address offset by x
 * @param cpu
 * @param mem
 * @return
 */
uint8_t zeropagex(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    return mem->readmem(cpu->pc)+cpu->x;
}


/**
 * Return zero page address offset by y
 * @param cpu
 * @param mem
 * @return
 */
uint8_t zeropagey(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    return mem->readmem(cpu->pc)+cpu->y;
}

/**
 * (Indirect,X) aka (zero page,X)
 * The address of the table is taken from the instruction and the X register added to it (with zero page wrap around)
 * to give the location of the least significant byte of the target address.
 * @param cpu
 * @param mem
 * @return Final resolved address
 */
uint16_t indexedindirect(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    uint8_t zpageaddr = mem->readmem(cpu->pc);
    uint16_t addr = zpageaddr + cpu->x;
    uint16_t finaladdr = revlendianbytes(mem->readmem(addr), mem->readmem(addr+1));
    return finaladdr;
}

/**
 * (Indirect),Y aka (zero page),Y
 * Y is added to final address at very end
 * @param cpu
 * @param mem
 * @return Final resolved address
 */
uint16_t indirectindexed(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    uint8_t zpageaddr = mem->readmem(cpu->pc);
    uint16_t addr = (revlendianbytes(mem->readmem(zpageaddr), mem->readmem(zpageaddr+1))) + cpu->y;
    return addr;
}

/**
 * Return absolute address offset by y
 * @param cpu
 * @param mem
 * @return
 */
uint16_t absolutey(cpustate * cpu, Memory * mem)
{
    cpu->pc+=2;
    uint16_t addr = revlendianbytes(mem->readmem(cpu->pc-1), mem->readmem(cpu->pc));
    addr += cpu->y;
    return addr;
}

/**
 * Return absolute address offset by x
 * @param cpu
 * @param mem
 * @return
 */
uint16_t absolutex(cpustate * cpu, Memory * mem)
{
    cpu->pc+=2;
    uint16_t addr = revlendianbytes(mem->readmem(cpu->pc-1), mem->readmem(cpu->pc));
    addr += cpu->x;
    return addr;
}
#include "system.h"

/*
 * This file includes function for the various addressing modes used by the 6502
 * http://www.emulator101.com/6502-addressing-modes.html
 * http://www.obelisk.me.uk/6502/addressing.html
 *
 * All functions in this file have the same inputs and outputs:
 * Take arguments of cpu and memory pointers
 * We assume that the pc is pointing at the current opcode
 * return final data (uint8_t)
 *
 * todo: double check these, unit test
 *
 * todo: update cycle count for every memory access
 */

// immediate addressing is passing the value specified in code
uint8_t immediate(cpustate * cpu, Memory * mem)
 {
     cpu->pc++;
     return mem->readmem(cpu->pc);
 }

uint8_t zeropage(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    uint8_t finaladdr = mem->readmem(cpu->pc+1);
    return mem->readmem(finaladdr);
}

uint8_t absolute(cpustate * cpu, Memory * mem)
{
    cpu->pc+=2;
    // is revlendianbytes() necessary?
    return mem->readmem(revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2)));
}

uint8_t zeropagex(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    return mem->readmem(mem->readmem(cpu->pc+1)+cpu->x);
}

uint8_t zeropagey(cpustate * cpu, Memory * mem)
{
    cpu->pc++;
    return mem->readmem(mem->readmem(cpu->pc+1)+cpu->y);
}

uint8_t indexedindirect(cpustate * cpu, Memory * mem)
{
    // (Indirect,X) aka (zero page,X)
    cpu->pc++;
    uint8_t zpageaddr = mem->readmem(cpu->pc+1);
    uint16_t addr = zpageaddr + cpu->x;
    // we have to read from addr to find the new addr to load from?
    uint16_t finaladdr = revlendianbytes(mem->readmem(addr), mem->readmem(addr+1));
    return mem->readmem(finaladdr);
}

uint8_t indirectindexed(cpustate * cpu, Memory * mem)
{
    // (Indirect),Y aka (zero page),Y
    cpu->pc++;
    uint8_t zpageaddr = mem->readmem(cpu->pc+1);
    uint16_t addr = (revlendianbytes(mem->readmem(zpageaddr), mem->readmem(zpageaddr+1))) + cpu->y;
    return mem->readmem(addr);
}

uint8_t absolutey(cpustate * cpu, Memory * mem)
{
    cpu->pc+=2;
    uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2));
    addr += cpu->y;
    return mem->readmem(addr);
}

uint8_t absolutex(cpustate * cpu, Memory * mem)
{
    cpu->pc+=2;
    uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2));
    addr += cpu->x;
    return mem->readmem(addr);
}
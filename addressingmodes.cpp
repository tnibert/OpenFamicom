#include "system.h"

/*
 * This file includes function for the various addressing modes used by the 6502
 * http://www.emulator101.com/6502-addressing-modes.html
 * Take arguments of cpu and memory pointers
 * We assume that the pc is pointing at the current opcode
 * return data
 */

uint8_t indexedindirect(cpustate * cpu, Memory * mem)
{
    uint8_t zpageaddr = mem->readmem(cpu->pc+1);
    uint16_t addr = zpageaddr + cpu->x;
    // we have to read from addr to find the new addr to load from?
    addr = revlendianbytes(mem->readmem(addr), mem->readmem(addr+1));
    return mem->readmem(addr);
}

uint8_t zeropage(cpustate * cpu, Memory * mem)
{
    return mem->readmem(mem->readmem(cpu->pc+1));
}

uint8_t indirectindexed(cpustate * cpu, Memory * mem)
{
    uint8_t zpageaddr = mem->readmem(cpu->pc+1);
    uint16_t addr = (revlendianbytes(mem->readmem(zpageaddr), mem->readmem(zpageaddr+1))) + cpu->y;
    return mem->readmem(addr);
}
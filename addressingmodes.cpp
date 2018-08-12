#include "system.h"

/*
 * This file includes function for the various addressing modes used by the 6502
 * http://www.emulator101.com/6502-addressing-modes.html
 * http://www.obelisk.me.uk/6502/addressing.html
 * Take arguments of cpu and memory pointers
 * We assume that the pc is pointing at the current opcode
 * return final location of data (uint16_t address)
 */

uint16_t indexedindirect(cpustate * cpu, Memory * mem)
{
    // (Indirect,X)
    uint8_t zpageaddr = mem->readmem(cpu->pc+1);
    uint16_t addr = zpageaddr + cpu->x;
    // we have to read from addr to find the new addr to load from?
    addr = revlendianbytes(mem->readmem(addr), mem->readmem(addr+1));
    return addr;
}

uint16_t zeropage(cpustate * cpu, Memory * mem)
{
    return mem->readmem(cpu->pc+1);
}

// this is a bit repetitive, think about a better design
uint16_t zeropagex(cpustate * cpu, Memory * mem)
{
    return mem->readmem(cpu->pc+1)+cpu->x;
}

uint16_t zeropagey(cpustate * cpu, Memory * mem)
{
    return mem->readmem(cpu->pc+1)+cpu->y;
}

uint16_t indirectindexed(cpustate * cpu, Memory * mem)
{
    // (Indirect),Y
    uint8_t zpageaddr = mem->readmem(cpu->pc+1);
    uint16_t addr = (revlendianbytes(mem->readmem(zpageaddr), mem->readmem(zpageaddr+1))) + cpu->y;
    return addr;
}

uint16_t absolutey(cpustate * cpu, Memory * mem)
{
    uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2));
    addr += cpu->y;
    return addr;
}

uint16_t absolutex(cpustate * cpu, Memory * mem)
{
    uint16_t addr = revlendianbytes(mem->readmem(cpu->pc+1), mem->readmem(cpu->pc+2));
    addr += cpu->x;
    return addr;
}
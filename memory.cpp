#include "system.h"

/*
 * This map will need some reworking
 * loading of prgrom, etc
 *
 * so 0000 - 0100 is zero page
 *    0100 - 0200 is the stack
 *    0200 - 0800 is the ram,
 *    then mirrors until 2000, repeating every 600
 *
    memory map (from https://wiki.nesdev.com/w/index.php/CPU_ALL):
    Address range	Size	Device
    $0000-$07FF	$0800	2KB internal RAM
    $0800-$0FFF	$0800	Mirrors of $0000-$07FF
    $1000-$17FF	$0800
    $1800-$1FFF	$0800
    $2000-$2007	$0008	NES PPU registers
    $2008-$3FFF	$1FF8	Mirrors of $2000-2007 (repeats every 8 bytes)
    $4000-$4017	$0018	NES APU and I/O registers
    $4018-$401F	$0008	APU and I/O functionality that is normally disabled. See CPU Test Mode.
    $4020-$FFFF	$BFE0	Cartridge space: PRG ROM, PRG RAM, and mapper registers (See Note)

    https://wiki.nesdev.com/w/index.php/Sample_RAM_map
    sample ram map for 2kb internal ram:
    $0000-$000F	16 bytes	Local variables and function arguments
    $0010-$00FF	240 bytes	Global variables accessed most often, including certain pointer tables
    $0100-$019F	160 bytes	Data to be copied to nametable during next vertical blank (see The frame and NMIs)
    $01A0-$01FF	96 bytes	Stack
    $0200-$02FF	256 bytes	Data to be copied to OAM during next vertical blank
    $0300-$03FF	256 bytes	Variables used by sound player, and possibly other variables
    $0400-$07FF	1024 bytes	Arrays and less-often-accessed global variables
*/

Memory::Memory()
{
    internalram = new uint8_t[0x800];
    ppureg = new uint8_t[0x0008];
    apuioreg = new uint8_t[0x0018];
    cputestmode = new uint8_t[0x0008];
    prg = new uint8_t[0xBFE0];
}

void MemoryAccessException(){

}

uint16_t convertmirror(uint16_t addr, int factor)
{
    // maybe this deserves its own function to convert mirror addresses, not sure yet
    return 0;
}

// todo: create unit tests

void Memory::writemem(uint16_t addr, uint8_t data)
{
    // There has to be a way to make this more efficient... luckily we are encapsulating it
    // everything is addr - offset
    // some of these && conditions can be done away with

    if(addr < 0x800)
    {
        // beautiful, we can directly access by provided address
        internalram[addr] = data;
    }
    else if(0x800 <= addr && addr < PPUREGSTART)
    {
        // not as beautiful, we are dealing with a mirror address
        // we mirror every 600 bytes
        // set should give us which iteration of 600 above 0x800 we are
        // then addr-(set*600) should convert us to the address under 0x800
        // this works because int division loses the remainder
        int set = addr/600;
        internalram[addr-(set*600)] = data;
    }
    else if(PPUREGSTART <= addr && addr < 0x2008)
    {
        // PPU registers
        ppureg[addr-PPUREGSTART] = data;
    }
    else if(0x2008 <= addr && addr < APUIOREGSTART)
    {
        // mirrors of PPU registers
        int set = 0x2008+(addr/8);
        internalram[addr-(set*8)] = data;
    }
    else if(APUIOREGSTART <= addr && addr < 0x4018)
    {
        // APU and IO registers
        apuioreg[addr-APUIOREGSTART] = data;
    }
    else if(CPUTESTMODESTART <= addr && addr < PRGROMSTART)
    {
        // CPU test mode
        cputestmode[addr-CPUTESTMODESTART] = data;
    }
    else if(PRGROMSTART <= addr)
    {
        // PRG area and mapper registers
        // come to think, we should never be writing to ROM...
        prg[addr-PRGROMSTART] = data;
    }

    //throw MemoryAccessException;

}
uint8_t Memory::readmem(uint16_t addr)
{
    if(addr < 0x800)
    {
        // beautiful, we can directly access by provided address
        return internalram[addr];
    }
    else if(0x800 <= addr && addr < PPUREGSTART)
    {
        // not as beautiful, we are dealing with a mirror address
        int set = addr/600;
        return internalram[addr-(set*600)];
    }
    else if(PPUREGSTART <= addr && addr < 0x2008)
    {
        // PPU registers
        return ppureg[addr-PPUREGSTART];
    }
    else if(0x2008 <= addr && addr < APUIOREGSTART)
    {
        // mirrors of PPU registers, repeats every 8 bytes
        int set = 0x2008+(addr/8);
        return internalram[addr-(set*8)];
    }
    else if(APUIOREGSTART <= addr && addr < 0x4018)
    {
        // APU and IO registers
        return apuioreg[addr-APUIOREGSTART];
    }
    else if(CPUTESTMODESTART <= addr && addr < PRGROMSTART)
    {
        // CPU test mode
        return cputestmode[addr-CPUTESTMODESTART];
    }
    else if(PRGROMSTART <= addr)
    {
        // PRG area and mapper registers
        return prg[addr-PRGROMSTART];
    }

    // if we get here, return error code or raise
    throw MemoryAccessException;
}

void Memory::loadprgrom(Cartridge * cart)
{
    // make prg a reference to the prgrom in cartridge
    prg = cart->prgrom;

    // copy data stored in cartridge prgrom to memory
    /*for(int i = 0; i < cart->prgromsize; i++)
    {
        prg[i] = cart->prgrom[i];                                     // this copies the data from cart->prgrom to memory[0x4020], not the reference
    }*/
}
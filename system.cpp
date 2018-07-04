#include "system.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <functional>
/*#include <memory>
#include <thread>
#include <chrono>
#include <mutex>*/

Famicom::Famicom(unsigned char * rom) {
    using namespace std;
    cout << "Famicom object is being created" << endl;

    // initialize cpu state
    cpu = (cpustate *)calloc(1, sizeof(cpustate));
    cpu->pc = 0;                         // may not start at 0
    // todo: verify all initial values
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->p = 0x34;
    cpu->sp = 0xfd;

    // initialize cartridge
    cart = new Cartridge(rom);

    // create memory map
    memory = new uint8_t[0xffff];         // 2 kB of onboard work RAM, cartridges can add, 2kb = 0x800
    /* todo: load game rom into memory @ $4020-$FFFF?
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
    opmap = create_opcode_map(cpu, memory);
}

opcode::opcode(uint8_t ocode, const char ocodestr[4])
{
    code = ocode;
    opcodestr = ocodestr;
}

void opcode::printcode()
{
    printf("%x\n", code);
}

std::map<uint8_t, std::shared_ptr<opcode> > create_opcode_map(cpustate * cpu, uint8_t * mem)
{
    std::map<uint8_t, std::shared_ptr<opcode> > opmap;
    std::shared_ptr<opcode> mycode;
    // now we need to do this for all opcodes
    //std::function<void ()> f = []() { std::cout <<  << std::endl; };
    for(uint8_t i = 0x0; i < 0xff; i++)
    {
        mycode = std::shared_ptr<opcode>(new opcode(i, "tes"));
        opmap[i] = mycode;
        opmap[i]->f = [mycode]() { printf("%x\n", mycode->code); };
    }

    // todo: create a function for the following assignment procedure
    // LDA $0314 constant
    // absolute address mode
    //delete &opmap[0xad];
    mycode = std::shared_ptr<opcode>(new opcode(0xad, "lda"));
    opmap[0xad] = mycode;
    opmap[0xad]->f = [cpu, mem, mycode]() {
        /*
         * I don't think this is quite correct, but we'll work with it
         * need to do a bit shift?
         * definitely not quite correct, we seg fault as soon as we hit this
         * probably because we haven't assigned anything to nes memory...
         */
        cpu->pc += 1;
        cpu->a = mem[cpu->pc];
        std::cout << mycode->opcodestr << " " << cpu->a << std::endl;
    };

    // dummy because we can't for loop up to 0xff on a uint8_t
    mycode = std::shared_ptr<opcode>(new opcode(0xff, "test"));
    opmap[0xff] = mycode;
    opmap[0xff]->f = [mycode]() { printf("%x\n", mycode->code); };

    return opmap;
}

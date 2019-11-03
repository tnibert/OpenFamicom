/*
 * http://nparker.llx.com/a2/opcodes.html
 */

#include "decoder.h"
#include "system.h"
#include <stdio.h>
#include <iostream>

/**
 * constructor
 */
InstructionDecoder::InstructionDecoder(cpustate * c, Memory * m)
{
    // register cpu state
    cpu = c;
    mem = m;
}

/**
 * Decode an opcode and execute it
 * form aaabbbcc or xxy10000
 * bbb and ccc match ups depends on cc
 * return number of cycles used
 */
int InstructionDecoder::decode_and_execute(uint8_t opcode)
{
    printf("In decode and execute: op: %x, pc: %x\n", opcode, cpu->pc);

    // todo: single byte instructions

    // The conditional branch instructions all have the form xxy10000.
    // The flag indicated by xx is compared with y, and the branch is taken if they are equal.
    if((opcode | 0b11100000) == 0b11110000)
    {
        uint8_t xx = opcode & 0b11000000;
        bool y = (opcode & 0b00100000) >> 5;
        // todo: finish this

        /*switch(xx)
        {

        }*/
    }
    else
    // form aaabbbcc instruction
    {
        uint8_t cc = opcode & 0b00000011;           // control code
        uint8_t bbb = opcode & 0b00011100;          // addressing mode
        uint8_t aaa = opcode & 0b11100000;          // operation
        uint16_t addr = NULL;                       // address of memory to access
        uint8_t data = NULL;                        // data to operate on

        // the following addressing mode codes are universal regardless of cc
        switch (bbb) {
            case 0b00000100: //always zero page
                addr = zeropage(cpu, mem);
                break;
            case 0b00001100: //always absolute
                addr = absolute(cpu, mem);
                break;
            case 0b00010100: //always zero page,X
                addr = zeropagex(cpu, mem);
                break;
            case 0b00011100: //always absolute,X
                addr = absolutex(cpu, mem);
                break;
        }

        switch (cc)                                  // evaluate control code
        {
            case 0b00:
                if (addr == NULL)
                {
                    switch (bbb)                        // evaluate addressing mode
                    {
                        case 0b00000000:
                            addr = immediate(cpu);
                    }
                }
                data = mem->readmem(addr);
                switch (aaa)
                {
                    case 0b00100000:
                        //BIT
                        break;
                    case 0b01000000:
                        //JMP
                        break;
                    case 0b01100000:
                        //JMP (abs) - Indirect
                        break;
                    case 0b10000000:
                        //STY
                        break;
                    case 0b10100000:
                        //LDY
                        break;
                    case 0b11000000:
                        //CPY
                        break;
                    case 0b11100000:
                        //CPX
                        break;
                }
                break;
            case 0b01:
                if (addr == NULL)
                {
                    switch (bbb)                        // evaluate addressing mode
                    {
                        case 0b00000000:
                            // (zero page,X) aka (indirect,X)
                            addr = indexedindirect(cpu, mem);
                            // will these inner breaks break out of the entire nested switch?
                            break;
                        case 0b00001000: // immediate
                            addr = immediate(cpu);
                            break;
                        case 0b00010000:
                            addr = zeropagey(cpu, mem);
                            break;
                        case 0b00011000:
                            addr = absolutey(cpu, mem);
                            break;
                    }
                }
                data = mem->readmem(addr);
                switch (aaa) {
                    case 0b00000000:
                        ORA(cpu, data);
                        break;
                    case 0b00100000:
                        AND(cpu, data);
                        break;
                    case 0b01000000:
                        EOR(cpu, data);
                        break;
                    case 0b01100000:
                        adc(cpu, data);
                        break;
                    case 0b10000000:
                        sta(cpu, mem, data);
                        break;
                    case 0b10100000:
                        ld(&cpu->a, cpu, data);
                        break;
                    case 0b11000000:
                        // todo: CMP
                        break;
                    case 0b11100000:
                        // SBC
                        sbc(cpu, data);
                        break;
                }

                break;

            case 0b10:
                if (addr == NULL)
                {
                    switch (bbb)                        // evaluate addressing mode
                    {
                        case 0b00000000:
                            addr = immediate(cpu);
                        case 0b00001000:
                            addr = cpu->a;
                    }
                }
                data = mem->readmem(addr);
                switch (aaa)
                {
                    case 0b00000000:
                        //asl
                        break;
                    case 0b00100000:
                        //rol
                        break;
                    case 0b01000000:
                        //lsr
                        break;
                    case 0b01100000:
                        //ror
                        break;
                    case 0b10000000:
                        //stx
                        break;
                    case 0b10100000:
                        //ldx
                        break;
                    case 0b11000000:
                        //dec
                        break;
                    case 0b11100000:
                        //inc
                        break;
                }
                break;

                // case 0b11 only used for illegal opcodes
        }
    }
    return 1;       // replace this with cycles used?
}
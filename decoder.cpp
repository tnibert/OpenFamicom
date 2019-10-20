/*
 * http://nparker.llx.com/a2/opcodes.html
 */

#include "decoder.h"
#include "system.h"

/**
 * constructor
 */
InstructionDecoder::InstructionDecoder(cpustate * c, Memory * mem)
{
    // register cpu state
    cpu = c;
}

/**
 * Decode an opcode and execute it
 * form aaabbbcc
 * bbb and ccc match ups depends on cc
 * todo: there is an issue with bit width mismatch
 */
void InstructionDecoder::decode_and_execute(uint8_t opcode)
{
    uint8_t cc = opcode & 0b00000011;           // control code
    uint8_t bbb = opcode & 0b00011100;          // addressing mode
    uint8_t aaa = opcode & 0b11100000;          // operation
    uint8_t data = NULL;

    // the following addressing mode codes are universal regardless of cc
    switch(bbb)
    {
        case 0b00000100: //always zero page
            data = zeropage(cpu, mem);
            break;
        case 0b00001100: //always absolute
            data = revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2));   // absolute, todo: is this correct?
            break;
        case 0b00010100: //always zero page,X
            data = zeropagex(cpu, mem);
            break;
        case 0b00011100: //always absolute,X
            data = absolutex(cpu, mem);
            break;
    }

    switch(cc)                                  // evaluate control code
    {
        case 0b00:
            if(data == NULL)
            {
                switch (bbb)                        // evaluate addressing mode
                {

                }
            }
            switch (aaa)
            {

            }
            break;
        case 0b01:
            if(data==NULL) {
                switch (bbb)                        // evaluate addressing mode
                {
                    // todo: need to ensure that all addressing functions are returning the final data from memory, clear format
                    // todo: add addressing functions for immediate and absolute
                    case 0b00000000:
                        // (zero page,X)
                        // will these inner breaks break out of the entire nested switch?
                        break;
                    case 0b00001000:
                        data = mem->readmem(cpu->pc + 1);
                        break;                                               // immediate, todo: is this correct?
                    case 0b00010000:
                        data = zeropagey(cpu, mem);
                        break;
                    case 0b00011000:
                        data = absolutey(cpu, mem);
                        break;
                }
            }
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
            // todo: update these functions for the 0b10 control codes
            if(data == NULL)
            {
                switch (bbb)                        // evaluate addressing mode
                {

                }
            }
            switch (aaa)
            {

            }
            break;

        // case 0b11 only used for illegal opcodes
    }

    // increment the program counter
    // todo: remove all other program counter increments in code
    // todo: do not increment counter in case of jump
    cpu->pc++;
}
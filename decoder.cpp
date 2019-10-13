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
void decode_and_execute(uint8_t opcode)
{
    uint8_t cc = opcode & 0b00000011;           // control code
    uint8_t bbb = opcode & 0b00011100;          // addressing mode
    uint8_t aaa = opcode & 0b11100000;          // operation
    uint8_t data = NULL;
    switch(cc)                                  // evaluate control code
    {
        case 0b01:
            break;
        case 0b01:
            switch (bbb)                        // evaluate addressing mode
            {
                case 0b000:
                    data = zeropagex(cpu, mem);
                    break;
                case 0b001:
                    data = zeropage(cpu, mem);
                    break;
                case 0b010:
                    data = mem->readmem(cpu->pc + 1);
                    break;                                               // immediate, todo: is this correct?
                case 0b011:
                    data = revlendianbytes(mem->readmem(cpu->pc + 1), mem->readmem(cpu->pc + 2));
                    break;       // absolute, todo: is this correct?
                case 0b100:
                    data = zeropagey(cpu, mem);
                    break;
                case 0b101:
                    data = zeropagex(cpu, mem);
                    break;
                case 0b110:
                    data = absolutey(cpu, mem);
                    break;
                case 0b111:
                    data = absolutex(cpu, mem);
                    break;
            }
            switch (ccc)
            {
                case 0b000:
                    ORA(cpu, data);
            }
            break;
        case 0b10:
            break;
        case 0b11:
            break;
    }
}
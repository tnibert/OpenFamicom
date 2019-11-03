#include "systemtests.h"
//#include <bitset>

using namespace CppUnit;

void TestDecoder::setUp()
{
    // initialize cpu state
    cpu = initcpu();
    mem = new Memory();
    decoder = new InstructionDecoder(cpu, mem);
}

void TestDecoder::test_aaabbbcc()
{
    // this may be a hack that we shouldn't allow
    // can't write to prgrom
    cpu->pc = 0;
    mem->writemem(0, 0x05);             // opcode
    mem->writemem(1, 2);                // store location in zero page
    mem->writemem(2, 0b10101010);       // zero page entry
    cpu->a = 0b01010101;

    // attempt zero page ORA 0x05
    decoder->decode_and_execute(mem->readmem(cpu->pc));
    CPPUNIT_ASSERT(cpu->a == 0b11111111);
}


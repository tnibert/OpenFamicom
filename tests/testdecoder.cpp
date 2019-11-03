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

void TestDecoder::test_branching()
{
    cpu->pc = 0;
    cpu->p = 0b00000010;
    //mem->writemem(0, 0xf0);
    mem->writemem(1, 3);                            // branch +3 bytes

    bool test = decoder->decode_branch(0xf0);       // decode BEQ
    CPPUNIT_ASSERT(test);

    cpu->p = 0b0;
    test = decoder->decode_branch(0xf0);
    CPPUNIT_ASSERT(!test);

    // todo: test the jumping
}
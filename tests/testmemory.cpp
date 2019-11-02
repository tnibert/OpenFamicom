#include "systemtests.h"

using namespace CppUnit;


void TestMemory::setUp(void)
{
    // initialize cpu state
    mem = new Memory();
}

void TestMemory::test_readwrite()
{
    // can we write to main 0x800 size memory area?
    mem->writemem(0x60f, 3);
    uint8_t test = mem->readmem(0x60f);
    CPPUNIT_ASSERT(test==3);

    // can we handle mirrored addresses?
    //System memory at $0000-$07FF is mirrored at $0800-$0FFF, $1000-$17FF, and $1800-$1FFF
    // attempting to access memory at, for example, $0173 is the same as accessing memory at $0973, $1173, or $1973.
    mem->writemem(0x1973, 1);
    test = mem->readmem(0x173);
    CPPUNIT_ASSERT(test==1);
    test = mem->readmem(0x1173);
    CPPUNIT_ASSERT(test==1);
    test = mem->readmem(0x973);
    CPPUNIT_ASSERT(test==1);

    // we should not be able to write to the prg rom area
    CPPUNIT_ASSERT_THROW( mem->writemem(0x5000, 10) , MemoryAccessException );
}
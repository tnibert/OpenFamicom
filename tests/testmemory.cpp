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

    // we should not be able to write to the prg rom area
    CPPUNIT_ASSERT_THROW( mem->writemem(0x5000, 10) , MemoryAccessException );
}
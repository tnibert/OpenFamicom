#include "systemtests.h"

using namespace CppUnit;


void TestMemory::setUp(void)
{
    // initialize cpu state
    mem = new Memory();
}

void TestMemory::test_readwrite()
{
    mem->writemem(0x60f, 3);
    uint8_t test = mem->readmem(0x60f);
    CPPUNIT_ASSERT(test==3);
}
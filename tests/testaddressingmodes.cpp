#include "systemtests.h"
using namespace CppUnit;


/**
 * Addressing mode functions will resolve the final addresses
 */

void TestAddressingModes::setUp(void)
{
    // initialize cpu state
    cpu = initcpu();
    mem = new Memory();
    cpu->pc = 0;
}

void TestAddressingModes::test_zeropage()
{
    //mem->writemem()
}

void TestAddressingModes::test_indexedindirect()
{
    // test normal case
    cpu->x = 0x4;
    mem->writemem(1, 0x20);
    mem->writemem(0x24, 0x74);
    mem->writemem(0x25, 0x20);
    CPPUNIT_ASSERT( indexedindirect(cpu, mem) == 0x2074 );

    // todo: test wrap around
}

void TestAddressingModes::test_indirectindexed()
{
    cpu->y = 0x10;
    mem->writemem(1, 0x86);
    mem->writemem(0x86, 0x28);
    mem->writemem(0x87, 0x40);
    CPPUNIT_ASSERT( indirectindexed(cpu, mem) == 0x4038 );
}

void TestAddressingModes::test_relative()
{
    // test positive branch
    cpu->pc++;
    mem->writemem(2, 80);
    cpu->pc = relative(cpu, mem);
    CPPUNIT_ASSERT( cpu->pc == 82);

    // test negative branch
    cpu->pc++;
    mem->writemem(84, -80);
    cpu->pc = relative(cpu, mem);
    CPPUNIT_ASSERT( cpu->pc == 4);

    // todo: what should happen if the branch goes below zero?
    // test wrap
    //cpu->pc++;
    //mem->writemem(6, -8);
    //cpu->pc = relative(cpu, mem);
    //std::cout << cpu->pc << std::endl;
}
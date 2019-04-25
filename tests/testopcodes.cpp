#include "systemtests.h"

using namespace CppUnit;
using namespace std;

// http://cppunit.sourceforge.net/doc/lastest/cppunit_cookbook.html

void TestOpCodes::setUp(void)
{
    // initialize cpu state
    cpu = initcpu();

    // create memory map
    memory = new Memory();

    opmap = create_opcode_map(cpu, memory);
}

void TestOpCodes::test_ORA_0x01_indirect_x()
{
    opmap[0x01]->f();
    CPPUNIT_ASSERT(false);
}


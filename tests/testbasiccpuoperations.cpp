#include "systemtests.h"

using namespace CppUnit;

// https://wiki.nesdev.com/w/index.php/Status_flags
// 0x34 for p register at init == 0b110100

void TestBasicCPUOperations::setUp(void)
{
    // initialize cpu state
    cpu = initcpu();
}

void TestBasicCPUOperations::test_ORA()
{
    uint8_t arg = 0b00000001;
    cpu->a = 0b00010000;
    ORA(cpu, arg);

    CPPUNIT_ASSERT(cpu->a == 0b00010001);
    CPPUNIT_ASSERT(cpu->p == 0b00110100);

    // test negative flag
    arg = 0b10000001;
    cpu->a = 0b00010000;
    ORA(cpu, arg);

    CPPUNIT_ASSERT(cpu->a == 0b10010001);
    CPPUNIT_ASSERT(cpu->p == 0b10110100);       // failing here

    // test zero flag
    arg = 0b00000000;
    cpu->a = 0b00000000;
    ORA(cpu, arg);

    CPPUNIT_ASSERT(cpu->a == 0b00000000);
    CPPUNIT_ASSERT(cpu->p == 0b00110110);
}
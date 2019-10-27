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
}

void TestAddressingModes::test_zeropage()
{
    //mem->writemem()
}
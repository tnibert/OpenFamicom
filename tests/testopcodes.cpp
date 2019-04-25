#include "../system.h"
#include "systemtests.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>

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


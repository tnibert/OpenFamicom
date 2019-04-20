#include "system.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
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

class TestOpCodes : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestBasicMath);

    public:
        void setUp(void)
        {
            // initialize cpu state
            cpustate * cpu = initcpu();

            // create memory map
            Memory * memory = new Memory();

            std::map<uint8_t, std::shared_ptr<opcode> > opmap = create_opcode_map(cpu, memory);
        }
        void tearDown(void);

};


CPPUNIT_TEST_SUITE_REGISTRATION( TestOpCodes );

int main(int argc, char* argv[])
{

}
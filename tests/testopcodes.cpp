#include "../system.h"

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

class TestOpCodes : public CppUnit::TestFixture
{
    //CPPUNIT_TEST_SUITE(TestBasicMath);

    private:
        cpustate * cpu;
        Memory * memory;
        std::map<uint8_t, std::shared_ptr<opcode> > opmap;

    public:
        void setUp(void)
        {
            // initialize cpu state
            cpu = initcpu();

            // create memory map
            memory = new Memory();

            opmap = create_opcode_map(cpu, memory);
        }
        static CppUnit::Test *suite()
        {
            CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "TestOpCodes" );
            suiteOfTests->addTest(new TestCaller <TestOpCodes> ("test_ORA_0x01", &TestOpCodes::test_ORA_0x01_indirect_x));
            return suiteOfTests;
        }
        //void tearDown(void);

        void test_ORA_0x01_indirect_x()
        {
            opmap[0x01]->f();
            CPPUNIT_ASSERT(false);
        }

};


//CPPUNIT_TEST_SUITE_REGISTRATION( TestOpCodes );

int main(int argc, char* argv[])
{
    /*
    TestSuite suite;
    TestResult result;

    suite.addTest(new TestCaller <TestOpCodes> test("test_ORA_0x01", &TestOpCodes::test_ORA-0x01_indirect_x));
    suite.run(&result);
     */
    cout << "check";
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( TestOpCodes::suite() );
    runner.run();
    return 0;
}

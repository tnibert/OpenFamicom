#ifndef OPENNES_SYSTEMTESTS_H
#define OPENNES_SYSTEMTESTS_H

#include "../system.h"
/*
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
*/
#include <cppunit/extensions/HelperMacros.h>

class TestOpCodes : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestOpCodes );
    CPPUNIT_TEST( test_ORA_0x01_indirect_x );
    CPPUNIT_TEST_SUITE_END();
    private:
        cpustate * cpu;
        Memory * memory;
        std::map<uint8_t, std::shared_ptr<opcode> > opmap;

    public:
        void setUp(void);
        //static CppUnit::Test *suite();

        void test_ORA_0x01_indirect_x();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestOpCodes );

#endif //OPENNES_SYSTEMTESTS_H


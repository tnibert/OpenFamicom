#ifndef OPENNES_SYSTEMTESTS_H
#define OPENNES_SYSTEMTESTS_H

#include "../system.h"
#include <cppunit/extensions/HelperMacros.h>

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


/**
 * These will test sharedinstruct.cpp
 */
class TestBasicCPUOperations : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestBasicCPUOperations );
    CPPUNIT_TEST( test_ORA );
    CPPUNIT_TEST_SUITE_END();

    private:
        cpustate * cpu;
    public:
        void setUp(void);
        //static CppUnit::Test *suite();

        void test_ORA();
        void test_LD();
        void test_STA();
        void test_STY();
        void test_STX();
        void test_ADC();
        void test_SBC();
        void test_AND();
        void test_EOR();
        void test_ASL();
        void test_LSR();
        void test_ROR();
        void test_ROL();
        void test_INC();
        void test_DEC();
        void test_compare();
        void test_BIT();
        void test_PUSH();
        void test_POP();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestBasicCPUOperations );

/**
 * Test addressing modes
 */
class TestAddressingModes : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestAddressingModes );
    CPPUNIT_TEST( test_immediate ); // add unit test
    CPPUNIT_TEST_SUITE_END();

    private:
        cpustate * cpu;
        Memory * mem;
    public:
        void setUp();
        void test_immediate();
        void test_zeropage();
        void test_absolute();
        void test_zeropagex();
        void test_zeropagey();
        void test_indexedindirect();
        void test_indirectindexed();
        void test_absolutey();
        void test_absolutex();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestAddressingModes );

#endif //OPENNES_SYSTEMTESTS_H


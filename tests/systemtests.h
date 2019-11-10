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

//Cartridge load_mock_cartridge();

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
    CPPUNIT_TEST( test_zeropage );
    CPPUNIT_TEST( test_indexedindirect );
    CPPUNIT_TEST( test_indirectindexed );
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
        void test_relative();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestAddressingModes );

/**
 * Test memory class
 */
class TestMemory : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestMemory );
    CPPUNIT_TEST( test_readwrite ); // add unit test
    CPPUNIT_TEST_SUITE_END();

private:
        Memory * mem;
    public:
        void setUp(void);
        void test_readwrite();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestMemory );

/**
 * Test decoder
 */
class TestDecoder : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestDecoder );
    CPPUNIT_TEST( test_aaabbbcc );
    CPPUNIT_TEST( test_branching );
    CPPUNIT_TEST_SUITE_END();

    private:
        cpustate * cpu;
        Memory * mem;
        InstructionDecoder * decoder;
    public:
        void setUp();
        void test_branching();
        void test_aaabbbcc();
        void test_singlebyte0x8();
        void test_singlebyte0xa();
        void test_brk();
        void test_rti();
        void test_jsrabs();
        void test_rts();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestDecoder );

class TestUtilFuncs : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestUtilFuncs );
    CPPUNIT_TEST( test_decode_twos_comp );
    CPPUNIT_TEST_SUITE_END();

    public:
        void test_decode_twos_comp();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestUtilFuncs );

#endif //OPENNES_SYSTEMTESTS_H


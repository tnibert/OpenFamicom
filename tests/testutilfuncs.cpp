#include "systemtests.h"
#include <bitset>
using namespace CppUnit;


void TestUtilFuncs::test_decode_twos_comp()
{
    // todo: assert fails
    //CPPUNIT_ASSERT( 0b10101011 == -85 );
    //std::cout << std::bitset<8>(decode_twos_comp(0b10101011)) << std::endl;
    CPPUNIT_ASSERT(decode_twos_comp(0b10101011) == -85);
    CPPUNIT_ASSERT(decode_twos_comp(0b01010101) == 85);
}

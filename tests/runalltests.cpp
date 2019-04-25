#include "../system.h"
#include "systemtests.h"

using namespace CppUnit;
using namespace std;


int main(int argc, char* argv[])
{
    // ok, this is running the test twice (3 times... blaaaah), I have no idea why :\
    cout << "check";
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool wasSuccessful = runner.run();
    return !wasSuccessful;
}
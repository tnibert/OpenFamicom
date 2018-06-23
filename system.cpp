#include "system.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

Famicom::Famicom(void) {
    cout << "Famicom object is being created" << endl;
    // initialize cpu state
    cpu = (cpustate *)calloc(1, sizeof(cpustate));
    //cpu->pc = ?;
}

OBJS = runrom.cpp system.cpp 6502.cpp cartridge.cpp memory.cpp opcodes.cpp sharedinstruct.cpp addressingmodes.cpp flags.cpp
TESTOBJS = tests/testopcodes.cpp system.cpp 6502.cpp cartridge.cpp memory.cpp opcodes.cpp sharedinstruct.cpp addressingmodes.cpp flags.cpp
#
all:
	g++ $(OBJS) -g -O0 -Wall -std=c++11 -o OpenFamicom

test:
	g++ $(TESTOBJS) -std=c++11 -lcppunit -o TestOpenFamicom
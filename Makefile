OBJS = runrom.cpp system.cpp 6502.cpp
#
all:
	g++ $(OBJS) -g -O0 -Wall -std=c++11 -o OpenFamicom

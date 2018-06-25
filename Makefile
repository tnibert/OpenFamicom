OBJS = runrom.cpp system.cpp
#
all:
	g++ $(OBJS) -g -O0 -Wall -std=c++11 -o OpenFamicom

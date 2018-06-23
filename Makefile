OBJS = runrom.cpp system.cpp
#
all:
	g++ $(OBJS) -g -O0 -Wall -o OpenFamicom

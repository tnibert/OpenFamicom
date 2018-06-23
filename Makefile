OBJS = system.cpp
#
all:
	cc $(OBJS) -g -O0 -Wall -o OpenFamicom

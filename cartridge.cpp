#include "system.h"
#include <iostream>
#include <stdio.h>

Cartridge::Cartridge(unsigned char *buffer)
{
    int readloc = 0;

    // process header
    // the 6502 is little endian, but I don't think I have to do anything funny to read the header of ines
    // http://nesdev.com/neshdr20.txt
    std::cout << "ines header being processed" << std::endl;
    for(;readloc < 16; readloc++)
    {
        header[readloc] = buffer[readloc];
    }

    // print out header for debug
    for(int i = 0; i < 16; i++) printf("%x", header[i]);
    std::cout << std::endl;
}

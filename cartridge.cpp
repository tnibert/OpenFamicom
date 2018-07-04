#include "system.h"
#include <iostream>
#include <stdio.h>

Cartridge::Cartridge(unsigned char *buffer)
{
    int readloc = 0;

    // process header
    // the 6502 is little endian, but I don't think I have to do anything funny to read the header of ines
    // http://nesdev.com/neshdr20.txt
    // https://wiki.nesdev.com/w/index.php/INES
    std::cout << "ines header being processed" << std::endl;
    for(;readloc < 16; readloc++)
    {
        header[readloc] = buffer[readloc];
    }

    // print out header for debug
    for(int i = 0; i < 16; i++) printf("%x", header[i]);
    std::cout << std::endl;

    // allocate prgrom and chrrom
    prgrom = new uint16_t[header[4]*1024];        // header specifies size of prgrom in 16kb segments
    chrrom = new uint8_t[header[5]*1024];         // header specified size of chrrom in 8kb segments

    // load flags
    uint8_t flags6 = header[6];
    uint8_t flags7 = header[7];
    uint8_t flags9 = header[9];
    uint8_t flags10 = header[10];

    // allocate prgram
    if(!header[8])
    {
        prgram = new uint8_t[8*1024];                          // if header says 0 for prgram, 8 kb for compatibility
    }
    else
    {
        prgram = new uint8_t[8*1024*header[8]];
    }

}

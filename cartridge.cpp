#include "system.h"
#include <iostream>
#include <stdio.h>

Cartridge::Cartridge(unsigned char *buffer)
{
    int readloc = 0;            // track our location in the buffer

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
    for(int i = 0; i < 16; i++) printf("%x-", header[i]);
    std::cout << std::endl;

    // load flags
    flags6 = header[6];
    flags7 = header[7];
    flags9 = header[9];
    flags10 = header[10];

    // do we have a trainer?
    if(flags6 & 0b00000100)
    {
        // if trainer exists, it is 512 bytes
        trainer = &buffer[readloc];         // correct usage of &?
        readloc += 512;
        printf("Trainer present\n");
    }
    else
    {
        trainer = NULL;
        printf("No trainer\n");
    }

    // allocate prgrom and chrrom

    //prgrom = new uint8_t[header[4]*1024*2];       // header specifies size of prgrom in 16kb segments
    printf("%x = %d 16kb segs for PRG ROM\n", header[4], header[4]);
    prgrom = &buffer[readloc];
    readloc += (header[4]*1024*2);                  // todo: store these sizes for prgrom and chrrom, although maybe header is good enough
    printf("PRG ROM size: %d bytes\n", header[4]*1024*2);

    //chrrom = new uint8_t[header[5]*1024];         // header specified size of chrrom in 8kb segments, value 0 means it uses chrram
    if(header[5])
    {
        chrrom = &buffer[readloc];
        readloc += (header[5]*1024);
        printf("CHR ROM present: %d bytes\n", header[5]*1024);
    }
    else                                            // we use chrram instead
    {
        chrrom = NULL;
        printf("No CHR ROM\n");
    }

    // allocate prgram
    if(!header[8])
    {
        prgram = new uint8_t[8*1024];                          // if header says 0 for prgram, 8 kb for compatibility
    }
    else
    {
        prgram = new uint8_t[8*1024*header[8]];                // do we create this or do we find this somewhere?
    }

    /*
     * after this there is
     * PlayChoice INST-ROM, if present (0 or 8192 bytes)
     * PlayChoice PROM, if present (16 bytes Data, 16 bytes CounterOut) (this is often missing, see PC10 ROM-Images for details)
     * We'll ignore for the moment
     */

}

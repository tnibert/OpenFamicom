#include "system.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char**argv)
{
    // first we load the rom file
    FILE *f= fopen(argv[1], "rb");
    if (f==NULL)
    {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    }

    //Get the file size and read it into a memory buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer = (unsigned char *)malloc(fsize);

    fread(buffer, fsize, 1, f);
    fclose(f);

    // create and init state machine
    Famicom * nes = new Famicom();
    //emustate->memory = malloc(0x10000);  //16K
    //emustate->pc = 0;

    return 0;
}

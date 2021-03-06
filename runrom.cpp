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
    Famicom * nes = new Famicom(buffer);


    /* so this is iterating over the prgrom */
    while (nes->getpc() < PRGROMSTART+nes->cart->prgromsize)
    {
        //printf("%x : %x\n", nes->getpc(), fsize);
        nes->emulate6502op();
    }
    printf("\n\n");
    return 0;
}

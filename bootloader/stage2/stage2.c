#include <stdint.h>

void main()
{
    volatile char* vmem = (volatile char*)0xB8000;
    char* text = "Stage 2 (C) executed";

    uint32_t i = 0;
    while ( *text )
    {
        vmem[i] = *text++;
        vmem[i+1] = 0x07;
        i += 2;
    }
}

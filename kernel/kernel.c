#include <stdint.h>

void main()
{
    uint32_t* vmem = (uint32_t*)0xFD000000;
    for ( int i = 0; i < 50; i++ )
        vmem[i] = 0x000000FF;

    while(1);
}

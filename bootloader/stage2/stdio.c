#include "stdio.h"

uint8_t btoa(uint8_t byte)
{
    byte += '0';
    if ( byte > 57 ) // Больше '9'
        byte += 7;
    return byte;
}

void putc(uint8_t c)
{
    __asm__ __volatile__
        (
            "int $0x10"
            :
            : "a" (0x0e00 | c), "b" (0x000f)
        );
}

void puts(const char* str)
{
    while(*str)
    {
        __asm__ __volatile__
            (
                "int $0x10"
                :
                : "a" (0x0e00 | *str++), "b" (0x000f)
            );
    }
}

void putn(const char* str, uint16_t size)
{
    while(size--)
    {
        __asm__ __volatile__
            (
                "int $0x10"
                :
                : "a" (0x0e00 | *str++), "b" (0x000f)
            );
    }
}

void nl()
{
    puts("\r\n");
}

void puthex8(uint8_t hex)
{
    uint8_t c = hex >> 4;
    putc(btoa(c));

    c = hex & 0x0F;
    putc(btoa(c));
}

void puthex16(uint16_t hex)
{
    puthex8(hex >> 8);
    puthex8(hex & 0x00FF);
}

void putdec8(uint8_t dec)
{
    uint8_t buffer[3] = { 32, 32, 32 }; // Spaces

    for ( uint8_t i = 0; i < 3; i++ )
    {
        if ( dec == 0 )
            break;
        buffer[2-i] = btoa(dec % 10);
        dec /= 10;
    }
    putn((char*)&buffer, 3);
}

void putdec16(uint16_t dec)
{
    uint8_t buffer[5] = { 32, 32, 32, 32, 32 }; // Spaces

    for ( uint8_t i = 0; i < 5; i++ )
    {
        if ( dec == 0 )
            break;

        buffer[4-i] = btoa(dec % 10);
        dec /= 10;
    }
    putn((char*)&buffer, 5);
}

#include "stdio.h"

uint8_t btoa(uint8_t byte)
{
    byte += '0';
    if ( byte > 57 ) // Больше '9'
        byte += 7;
    return byte;
}

uint8_t atob(char c)
{
    if ( c >= '0' && c <= '9' )
        return (uint8_t)(c - 48);

    if ( c >= 'A' && c <= 'F' )
        return (uint8_t)(c - 65 + 10);

    if ( c >= 'a' && c <= 'f' )
        return (uint8_t)(c - 97 + 10);

    return 0;
}

uint8_t sdec8tob(char* str)
{
    uint8_t result = 0;

    uint8_t size = 0;
    while ( *str && *str != 0x0D )
    {
        size++;
        str++;
    }
    str--;

    uint8_t power = 1;
    for ( uint8_t i = 0; i < size; i++ )
    {
        result += atob(*(str--)) * power;
        power *= 10;
    }

    return result;
}

uint16_t sdec16tob(char* str)
{
    uint16_t result = 0;

    uint16_t size = 0;
    while ( *str && *str != 0x0D )
    {
        size++;
        str++;
    }
    str--;

    uint16_t power = 1;
    for ( uint16_t i = 0; i < size; i++ )
    {
        result += atob(*(str--)) * power;
        power *= 10;
    }

    return result;
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

char getc()
{
    char result;
    __asm__ __volatile__
        (
            "int $0x16\n"
            "movb %%al, %0"
            : "=r" (result)
            : "a" (0x0000)
        );
    return result & 0x00FF;
}

void gets(char* buffer)
{
    char c;
    do
    {
        c = getc();
        *(buffer++) = c;
        putc(c);
    } while ( c != 0x0D );
    *buffer = 0;
}

#include <stdint.h>
#include "stdio.h"
#include "vbe.h"

typedef struct
{
    uint16_t number;
    uint16_t width;
    uint16_t height;
    uint8_t  bpp;
    uint32_t vmem;
} VideoMode;

VideoMode selected_mode;

void main()
{
    char* text = "Stage 2 (C) executed\r\n";
    puts(text);

    // Чтение информации о VBE
    VBEInfo vbeinfo;
    if ( read_vbeinfo (&vbeinfo) != 0x004F )
    {
        puts(" [Error] while reading VBE info\r\n");
        return;
    }

    puts("VBE info loaded. Signature: ");
    putn((char*)vbeinfo.signature, 4);
    nl();
    puts("Select video mode to use:\r\n");

    VideoMode mode_buffer[2048];
    uint16_t mode_count = 0;

    uint32_t mode_list = (uint32_t)(vbeinfo.video_modes_ptr);
    uint16_t segment = mode_list >> 16;   // Старшие 16 бит - сегмент
    uint16_t offset = (mode_list & 0xFFFF); // Младшие 16 бит - смещение

    // Проход по всем доступным режимам
    while (1)
    {
        /* uint16_t segment = mode_list >> 16;   // Старшие 16 бит - сегмент */
        /* uint16_t offset = (mode_list & 0xFFFF); // Младшие 16 бит - смещение */
    
        /* // Вычисляем линейный адрес */
        /* uint32_t linear_address = ((uint32_t)segment << 4) + offset; */
        /* // Получаем 16-битное значение */
        /* uint16_t mode_number = *(uint16_t*)linear_address; */

        uint16_t mode_number = 0;
        __asm__ __volatile__
            (
                "movw %%ax, %%fs\n"
                "movw %%fs:(%%bx), %0\n"
                : "=r" (mode_number)
                : "a" (segment), "b" (offset)
            );


        if ( mode_number == 0xFFFF || mode_count >= 2045 )
            break;

        // Получение информации о режиме
        VBEModeInfo modeinfo;
        if ( read_mode_info (mode_number, &modeinfo) != 0x004F )
            /* { mode_list += 0x02; continue; } */
            { offset += 0x02; continue; }

        // Если не 32 бита на пиксель, переходим к следующему
        if ( modeinfo.bits_per_pixel != 32 )
            /* { mode_list += 0x02; continue; } */
            { offset += 0x02; continue; }

        // Если не линейный буффер, переходим к следующему
        if ( !(modeinfo.attributes & 0x80) )
            /* { mode_list += 0x02; continue; } */
            { offset += 0x02; continue; }

        uint16_t width = modeinfo.width; 
        uint16_t height = modeinfo.height; 
        uint8_t  bpp = modeinfo.bits_per_pixel;
        uint32_t vmem = modeinfo.linear_buffer_addr;

        // Пропуск схожих с уже найденными
        uint8_t found = 0;
        for ( uint16_t i = 0; i < mode_count; i++ )
        {
            if ( mode_buffer[i].width == width && 
                 mode_buffer[i].height == height &&
                 mode_buffer[i].bpp == bpp)
            {
                found = 1;
                break;
            }
        }
        if ( found )
            /* { mode_list += 0x02; continue; } */
            { offset += 0x02; continue; }

        // Сохранение режима в буфер
        mode_buffer[mode_count++] = (VideoMode){ .number=mode_number,
                                                 .width=width,
                                                 .height=height,
                                                 .bpp=bpp,
                                                 .vmem=vmem};

        // Вывод основной информации о режиме
        putdec16(mode_count);
        puts(") Mode 0x");
        puthex16(mode_number);
        puts(" --- Resolution: ");
        putdec16(width);
        puts(" x");
        putdec16(height);
        puts(" --- FB addr: 0x");
        puthex16(vmem>>16);
        puthex16(vmem&0xFFFF);
        nl();

        /* mode_list += 0x02; */
        offset += 0x02;
    }

    puts("Enter mode number to select: ");
    char buffer[32] = {0};
    gets((char*)&buffer);
    nl();

    uint16_t sel = sdec16tob((char*)&buffer);
    if ( select_mode(mode_buffer[sel-1].number) != 0x004F )
    {
        puts("[Error] Unable to select mode!\r\n");
        return;
    }

    selected_mode = mode_buffer[sel-1];
    puts("Mode successfully selected!\r\n");

    return;
}

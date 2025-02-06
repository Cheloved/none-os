#include "vbe.h"

VideoMode selected_mode;

uint16_t read_vbeinfo(VBEInfo* vbeinfo)
{
    uint16_t return_code;
    __asm__ __volatile__
        (
            "mov %%bx, %%di\n"
            "int $0x10\n"
            "movw %%ax, %0"
            : "=r" (return_code)
            : "a" (0x4F00), "b" (vbeinfo)
        );
    return return_code;
}

uint16_t read_mode_info(uint16_t number, VBEModeInfo* modeinfo)
{
    uint16_t return_code;
    __asm__ __volatile__
        (
            "mov %%bx, %%di\n"
            "int $0x10\n"
            "movw %%ax, %0"
            : "=r" (return_code)
            : "a" (0x4F01), "b" (modeinfo), "c" (number)
        );
    return return_code;
}

uint16_t select_mode(uint16_t number)
{
    uint16_t return_code;
    __asm__ __volatile__
        (
            "orw $0x4000, %%bx\n"
            "int $0x10\n"
            "movw %%ax, %0"
            : "=r" (return_code)
            : "a" (0x4F02), "b" (number)
        );
    return return_code;
}

uint8_t user_select_mode()
{
    // Чтение информации о VBE
    VBEInfo vbeinfo;
    if ( read_vbeinfo (&vbeinfo) != 0x004F )
    {
        puts(" [Error] while reading VBE info\r\n");
        return -1;
    }

    puts("VBE info loaded. Signature: ");
    putn((char*)vbeinfo.signature, 4);
    nl();
    puts("Select video mode to use:\r\n");

    VideoMode mode_buffer[2048];
    uint16_t mode_count = 0;

    uint32_t mode_list = (uint32_t)(vbeinfo.video_modes_ptr);
    uint16_t mode_number = 0;

    // Проход по всем доступным режимам
    for ( uint32_t mode_list = (uint32_t)(vbeinfo.video_modes_ptr);
          mode_number = get_far_value(mode_list) != 0xFFFF; mode_list += 0x02)
    {
        mode_number = get_far_value(mode_list);
        
        if ( mode_number == 0xFFFF || mode_count >= 2048 )
            break;

        // Получение информации о режиме
        VBEModeInfo modeinfo;
        if ( read_mode_info (mode_number, &modeinfo) != 0x004F )
            continue;

        // Если не 32 бита на пиксель, переходим к следующему
        if ( modeinfo.bits_per_pixel != 32 )
            continue;

        // Если не линейный буффер, переходим к следующему
        if ( !(modeinfo.attributes & 0x80) )
            continue;

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
            continue;

        // Сохранение режима в буфер
        mode_buffer[mode_count++] = (VideoMode){ .number=mode_number,
                                                 .width=width,
                                                 .height=height,
                                                 .bpp=bpp,
                                                 .vmem=(uint32_t*)vmem};

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
    }

    puts("Enter mode number to select: ");
    char buffer[32] = {0};
    gets((char*)&buffer);
    nl();

    uint16_t sel = sdec16tob((char*)&buffer);
    if ( select_mode(mode_buffer[sel-1].number) != 0x004F )
    {
        puts("[Error] Unable to select mode!\r\n");
        return -1;
    }

    selected_mode = mode_buffer[sel-1];
    return 0;
}

uint16_t get_far_value(uint32_t ptr)
{
    uint16_t offset = (ptr & 0xFFFF);   // Младшие 16 бит - смещение
    uint16_t segment = ptr >> 16;       // Старшие 16 бит - сегмент

    uint16_t result = 0;
    __asm__ __volatile__
        (
            "movw %%ax, %%fs\n"
            "movw %%fs:(%%bx), %0\n"
            : "=r" (result)
            : "a" (segment), "b" (offset)
        );
    return result;
}

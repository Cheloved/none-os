#include <stdint.h>
#include "stdio.h"
#include "vbe.h"

void main()
{
    char* text = "Stage 2 (C) executed\r\n";
    puts(text);

    // Чтение информации о VBE
    VBEInfo vbeinfo;
    if ( read_vbeinfo (&vbeinfo) != 0x004F )
        puts(" [Error] while reading VBE info\r\n");

    uint16_t mode_buffer[256];
    uint16_t mode_count = 0;

    // Проход по всем доступным режимам
    for ( uint16_t* mode_list = (uint16_t*)(vbeinfo.video_modes_ptr);
          *mode_list != 0xFFFF; mode_list++)
    {
        // Получение информации о режиме
        VBEModeInfo modeinfo;
        read_mode_info(*mode_list, &modeinfo);

        // Если не 32 бита на пиксель, переходим к следующему
        if ( modeinfo.bits_per_pixel != 32 )
            continue;

        // Если не линейный буффер, переходим к следующему
        if ( !(modeinfo.attributes & 0x80) )
            continue;

        // Сохранение номера в буффер
        mode_buffer[mode_count++] = *mode_list;

        // Вывод основной информации о режиме
        putdec16(mode_count);
        puts(") Mode 0x");
        puthex16(*mode_list);
        puts(" --- Resolution: ");
        putdec16(modeinfo.width);
        puts(" x");
        putdec16(modeinfo.height);
        nl();
    }

    puts("Enter mode number to select: ");
    char buffer[32] = {0};
    gets((char*)&buffer);
    nl();

    uint16_t sel = sdec16tob((char*)&buffer);
    if ( select_mode(mode_buffer[sel-1]) != 0x004F )
        puts("[Error] Unable to select mode!\r\n");

    puts("Mode successfully selected!");
}

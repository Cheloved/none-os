#include <stdint.h>
#include "stdio.h"
#include "vbe.h"

void main()
{
    char* text = "Stage 2 (C) executed\r\n";
    puts(text);

    VBEInfo vbeinfo;
    if ( read_vbeinfo (&vbeinfo) != 0x004F )
    {
        puts(" [Error] while reading VBE info\r\n");
    }

    uint16_t* mode_list = (uint16_t*)(vbeinfo.video_modes_ptr);
    for ( uint16_t* mode_list = (uint16_t*)(vbeinfo.video_modes_ptr);
          *mode_list != 0xFFFF; mode_list++)
    {
        VBEModeInfo modeinfo;
        read_mode_info(*mode_list, &modeinfo);

        if ( modeinfo.bits_per_pixel != 32 )
            continue;

        puts("Mode 0x");
        puthex16(*mode_list);
        puts(" --- Resolution: ");
        putdec16(modeinfo.width);
        puts(" x");
        putdec16(modeinfo.height);
        nl();
    }
}

#include <stdint.h>
#include "stdio.h"
#include "vbe.h"
#include "protected.h"
#include "fat.h"

void main()
{
    puts("Stage 2 (C) executed\r\n");

    if ( !user_select_mode() )
        puts("Video mode set\r\n");
    else
        puts("Error setting video mode\r\n");

    puts("Reading FAT table\r\n");
    read_boot_data();
    puts((char*)bd.oem);
    nl();
    read_root_dir();
    for ( uint16_t i = 0; i < file_count; i++ )
    {
        puts("  Found file: ");
        putn(files[i].filename, 8);
        putn(files[i].extension, 3);
        nl();
    }

    /* if ( !load_file("STAGE2  ", "BIN", KERNEL_BASE) ) */
    if ( !load_file("KERNEL  ", "BIN", KERNEL_BASE) )
        puts("Kernel successfully loaded at 0x02000000");
    else
        puts("Error reading kernel");

    /* puts("Entering protected mode\r\n"); */
    /* init_gdt(); */
    while(1);
    return;
}

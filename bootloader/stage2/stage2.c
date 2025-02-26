#include <stdint.h>
#include "stdio.h"
#include "vbe.h"
#include "protected.h"
#include "fat.h"
#include "multiboot.h"
#include "fetching.h"

MultibootInfo Multiboot;
Info info;

void __attribute__((cdecl)) main()
{
    puts("Stage 2 (C) executed\r\n");

    if ( !user_select_mode() )
        puts("Video mode set\r\n");
    else
        puts("Error setting video mode\r\n");

    puts("  Framebuffer addr: 0x"); puthex16((uint32_t)selected_mode.vmem >> 16); puthex16((uint32_t)selected_mode.vmem & 0xFFFF); nl();

    puts("Reading FAT table\r\n");
    read_boot_data();

    putn((char*)bd.ebr_volume_label, 11);
    nl();

    read_root_dir();
    for ( uint16_t i = 0; i < file_count; i++ )
    {
        puts("  Found file: ");
        putn(files[i].filename, 8);
        putn(files[i].extension, 3);
        nl();
    }

    uint8_t err = 0;
    err = load_file("KERNEL  ", "BIN", KERNEL_BASE);
    if ( err == 1 )
        { puts("Error! Kernel.bin not found\r\n"); return; }
    if ( err == 2 )
        { puts("Error reading kernel\r\n"); return; }
        
    puts("Kernel successfully loaded at "); puthex16(KERNEL_BASE>>16); puts(":");
    puthex16(KERNEL_BASE&0xFFFF); nl();

    puts("Entering protected mode\r\n");
    puts("  init_gdt() address: "); puthex16((uint16_t)init_gdt); nl();
    puts("  kernel_jump() address: "); puthex16((uint16_t)kernel_jump); nl();
    init_gdt();

    // Сбор данных
    info = *((Info*)&selected_mode);
    fetch_multiboot(&Multiboot);

    // Переход к выполнению кода в защищенном режиме
    puts("Long jump to kernel\r\n");
    kernel_jump();

    puts("After\r\n");
    while(1);
    return;
}

#include <stdint.h>

#include "../bootloader/stage2/multiboot.h"
#include "stdio.h"
#include "disk.h"

MultibootInfo* mbi;
Info* inf;
uint32_t magic;

void main()
{
    __asm__ __volatile__
        (
            "mov %%eax, %0\n"
            "mov %%ebx, %1\n"
            "mov %%edx, %2\n"
            :  "=m" (mbi), "=m" (inf), "=m" (magic)
            :
            : "memory"
        );

    clear_screen();
    set_color(COLOR_WHITE);
    puts("Kernel successfully loaded!\n");

    if ( magic == MULTIBOOT_BOOTLOADER_MAGIC )
    {
        puts("Multiboot magic number is correct: ");
        puthex(magic);
        putc('\n');
    }
    else
    {
        puts("Warning: multiboot magic number is NOT correct\n");
        puts("         should be: "); puthex(MULTIBOOT_BOOTLOADER_MAGIC);
        puts("\n         got:       "); puthex(magic);
    }

    puts("Reading FAT table from RAM\r\n");
    read_boot_data();
    read_root_dir();
    for ( uint16_t i = 0; i < file_count; i++ )
    {
        puts("  Found file: ");
        putn(files[i].filename, 8);
        putn(files[i].extension, 3);
        nl();
    }

    puts("Initializing IDE\r\n");
    init_ide();

    uint16_t text_first = get_file_first_cluster("TEXT    ", "TXT");
    puts("Text file first cluster: ");
    putdec(text_first);
    nl();

    uint16_t lba = 33 + (text_first-2) * bd.sectors_per_cluster;
    char buffer[512];

    puts("Content of TEXT.TXT:\n");
    ide_read_sector(lba, (uint8_t*)&buffer);
    puts("Content of TEXT.TXT:\n");
    putn(buffer, 20);

    while(1);
}

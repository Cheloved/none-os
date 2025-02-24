#include <stdint.h>

#include "../bootloader/stage2/multiboot.h"
#include "stdio.h"
#include "disk.h"
#include "interrupts.h"
#include "kbd.h"

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
        printf("Multiboot magic number is correct: %x\n", magic);
    else
        printf("Warning: multiboot magic number is NOT correct\n"
               "         should be: %x\n"
               "         got:       %x\n", MULTIBOOT_BOOTLOADER_MAGIC, magic);

    struct MMapEntry{ uint32_t base_low, base_high, len_low, len_high, type; } MMapEntry;
    /* puts("Multiboot data:\n"); */
    printf("Multiboot data:\n");
    printf("    Flags: %b\n", mbi->flags);
    printf("    Lower mem size: %d kb\n", mbi->mem_lower);
    printf("    Upper mem size: %d kb\n", mbi->mem_upper);
    printf("    Boot device number: %d (%x)\n", mbi->boot_device, mbi->boot_device);
    printf("    Mmap length:  %d\n", mbi->mmap_length);
    printf("    Mmap entries: %d\n", mbi->mmap_length / sizeof(MMapEntry));
    printf("    Mmap addr:    %x\n", mbi->mmap_addr);
    puts("    Bootloader name: "); putn((char*)mbi->boot_loader_name, 4); nl();
    printf("    Framebuffer addr low:  %x\n", mbi->framebuffer_addr_low);
    printf("    Framebuffer addr high: %x\n", mbi->framebuffer_addr_high);
    printf("    Framebuffer width:     %d\n", mbi->framebuffer_width);
    printf("    Framebuffer height:    %d\n", mbi->framebuffer_height);
    printf("    Framebuffer bpp:       %d\n", mbi->framebuffer_bpp);
    printf("    Framebuffer type:      %d\n", mbi->framebuffer_type);


    puts("Reading FAT table from RAM\r\n");
    read_boot_data();
    read_root_dir();
    for ( uint16_t i = 0; i < file_count; i++ )
    {
        puts(" ---- Found file: ");
        putn(files[i].filename, 8);
        putn(files[i].extension, 3);
        nl();
    }

    uint16_t text_first = get_file_first_cluster("TEXT    ", "TXT");
    puts("Text file first cluster: ");
    putdec(text_first);
    nl();

    uint16_t lba = 33 + (text_first-2) * bd.sectors_per_cluster;
    char buffer[512];

    ata_init();
    ata_read(lba, buffer);
    puts("Content of TEXT.TXT:\n");
    putn(buffer, 20);

    // Установкa базовых векторов прерываний:
    // IRQ0–IRQ7: 0x20–0x27
    // IRQ8–IRQ15: 0x28–0x2F
    puts("Remapping PIC\n");
    pic_remap(0x20, 0x28);

    // Составление и загрузка IDT
    puts("Initializing IDT\n");
    idt_init();

    asm volatile("sti");

    printf("\n\n   Testing printf: %d, %x, %b, %c\n\n", 123, 123, 123, 'A'); 

    puts("Main loop entered\n");
    while(1)
    {
        putc(getc());
    }
}

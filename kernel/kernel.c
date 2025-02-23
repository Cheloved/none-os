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

    struct MMapEntry{ uint32_t base_low, base_high, len_low, len_high, type; } MMapEntry;
    puts("Multiboot data:\n");
    puts(" ---- Flags: "); puthex(mbi->flags); nl();
    puts(" ---- Lower mem size: "); putdec(mbi->mem_lower); puts("kb"); nl();
    puts(" ---- Upper mem size: "); putdec(mbi->mem_upper); puts("kb"); nl();
    puts(" ---- Boot device number: "); puthex(mbi->boot_device); nl();
    puts(" ---- Mmap length:  "); putdec(mbi->mmap_length); nl();
    puts(" ---- Mmap entries: "); putdec(mbi->mmap_length / sizeof(MMapEntry)); nl();
    puts(" ---- Mmap addr:    "); puthex(mbi->mmap_addr); nl();
    puts(" ---- Bootloader name: "); putn((char*)mbi->boot_loader_name, 4); nl();
    puts(" ---- Framebuffer addr low:  "); puthex(mbi->framebuffer_addr_low); nl();
    puts(" ---- Framebuffer addr high: "); puthex(mbi->framebuffer_addr_high); nl();
    puts(" ---- Framebuffer width:   "); putdec(mbi->framebuffer_width); nl();
    puts(" ---- Framebuffer height:  "); putdec(mbi->framebuffer_height); nl();
    puts(" ---- Framebuffer bpp:     "); putdec(mbi->framebuffer_bpp); nl();
    puts(" ---- Framebuffer type:      "); puthex(mbi->framebuffer_type); nl();


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

    puts("Main loop entered\n");
    while(1)
    {
        putc(getc());
    }
}

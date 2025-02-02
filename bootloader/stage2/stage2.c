#include <stdint.h>
#include "stdio.h"
#include "vbe.h"
#include "protected.h"

void main()
{
    puts("Stage 2 (C) executed\r\n");

    if ( !user_select_mode() )
        puts("Video mode set\r\n");
    else
        puts("Error setting video mode\r\n");

    puts("Entering protected mode\r\n");

    puts("GDT entry size: ");
    puthex16(sizeof(GDT_entry));
    nl();
    puts("GDT size: ");
    puthex16(current_gdt_desc.size);
    nl();
    puts("GDT start: ");
    puthex16(current_gdt_desc.start >> 16);
    puthex16(current_gdt_desc.start & 0xFFFF);
    nl();
    puts("Code segment: ");
    puthex16(CODE_SEG);
    nl();
    puts("Data segment: ");
    puthex16(DATA_SEG);
    nl();
    getc();

    disable_a20();
    load_gdt();
    goto_protected();

    while(1);
    return;
}

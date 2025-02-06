#include <stdint.h>

#include "../bootloader/stage2/multiboot.h"
#include "stdio.h"

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

    while(1);
}

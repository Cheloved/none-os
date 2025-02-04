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
    init_gdt();

    while(1);
    return;
}

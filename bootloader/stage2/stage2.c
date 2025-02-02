#include <stdint.h>
#include "stdio.h"
#include "vbe.h"

void main()
{
    puts("Stage 2 (C) executed\r\n");
    /* user_select_mode(); */
    while ( 1 )
    {
        if ( !user_select_mode() )
            puts("Error setting video mode\r\n");
        else
            puts("Video mode set\r\n");

    }
    while(1);
    return;
}

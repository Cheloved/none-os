#include "vbe.h"

uint16_t read_vbeinfo(VBEInfo* vbeinfo)
{
    uint16_t return_code;
    __asm__ __volatile__
        (
            "mov %%bx, %%di\n"
            "int $0x10\n"
            "movw %%ax, %0"
            : "=r" (return_code)
            : "a" (0x4F00), "b" (vbeinfo)
        );
    return return_code;
}

uint16_t read_mode_info(uint16_t number, VBEModeInfo* modeinfo)
{
    uint16_t return_code;
    __asm__ __volatile__
        (
            "mov %%bx, %%di\n"
            "int $0x10\n"
            "movw %%ax, %0"
            : "=r" (return_code)
            : "a" (0x4F01), "b" (modeinfo), "c" (number)
        );
    return return_code;
}

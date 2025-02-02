#pragma once

#include <stdint.h>
#include "vbe.h"

// === Определение GDT === //

typedef struct
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  flags_type;
    uint8_t  flags_other;
    uint8_t  base_high;
} __attribute__((packed)) GDT_entry;

typedef struct
{
    uint32_t  null_descriptor;
    GDT_entry code_descriptor;
    GDT_entry data_descriptor;
} __attribute__((packed)) GDT;

typedef struct
{
    uint16_t size;
    uint32_t start;
} __attribute__((packed)) GDT_descriptor;

GDT current_gdt = (GDT)
{
    .null_descriptor = 0,
    .code_descriptor = (GDT_entry)
    {
        .limit_low   = 0xFFFF,      // Лимит(размер) (0-15)
        .base_low    = 0x0000,      // База (начало) (0-15)
        .base_middle = 0x00,        // База (16-23)
        .flags_type  = 0b10011010,  // Present   = 1  - для используемых сегментов
                                    // Privilege = 00 - "ring"
                                    // Type      = 1  - code/data
                                    // Type flags:
                                    //     1 - code
                                    //     0 - conforming
                                    //     1 - readable
                                    //     0 - accessed ( устанавливается CPU )
        .flags_other = 0b11001111,  // Other flags: 
                                    //     1    - granularity ( limit *= 16 )
                                    //     00   - для AVL (не используется) 
                                    //     1111 - лимит (16-23)
        .base_high   = 0b0          // База (24)
    },
    .data_descriptor = (GDT_entry)
    {
        .limit_low   = 0xFFFF,      // Лимит(размер) (0-15)
        .base_low    = 0x0000,      // База (начало) (0-15)
        .base_middle = 0x00,        // База (16-23)
        .flags_type  = 0b10010010,  // Present   = 1  - для используемых сегментов
                                    // Privilege = 00 - "ring"
                                    // Type      = 1  - code/data
                                    // Type flags:
                                    //     0 - data
                                    //     0 - conforming
                                    //     1 - readable
                                    //     0 - accessed ( устанавливается CPU )
        .flags_other = 0b11001111,  // Other flags: 
                                    //     1    - granularity ( limit *= 16 )
                                    //     00   - для AVL (не используется) 
                                    //     1111 - лимит (16-23)
        .base_high   = 0b0          // База (24)
    },
};

GDT_descriptor current_gdt_desc = (GDT_descriptor)
{
    .size  = sizeof(current_gdt),
    .start = (uint32_t)&current_gdt
};

// Номера сегментов
uint16_t CODE_SEG = (uint16_t)&current_gdt.code_descriptor - (uint16_t)&current_gdt;
uint16_t DATA_SEG = (uint16_t)&current_gdt.data_descriptor - (uint16_t)&current_gdt;

void __attribute__((cdecl)) disable_a20();
void __attribute__((cdecl)) load_gdt();
void __attribute__((cdecl)) goto_protected();

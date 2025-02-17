#pragma once

#include <stdint.h>
#include "stdio.h"
#include "ports.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define IDT_ENTRIES 256

#define KEYBOARD_DATA_PORT 0x60

typedef struct 
{
   uint16_t offset_low;      // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  zero;            // unused, set to 0
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_high;     // offset bits 16..31
} __attribute__((packed)) InterruptDescriptor;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDT_ptr_t;

extern InterruptDescriptor idt[IDT_ENTRIES];
extern IDT_ptr_t idt_ptr;

void idt_set_entry(int index, uint32_t base, uint16_t selector, uint8_t flags);
void idt_init();

void pic_remap(int offset1, int offset2);

extern void __attribute__((cdecl)) _default_int_handler();
extern void __attribute__((cdecl)) _keyboard_int_handler();
void keyboard_handler();

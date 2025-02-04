#pragma once

#include <stdint.h>
#include "vbe.h"

// === Определение GDT === //
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) GDT_entry;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) GDT_descriptor;

extern GDT_entry current_gdt[3];
extern GDT_descriptor current_gdt_desc;

// Проверка перехода в 32-битный режим
int is_32bit_mode();

// Функция для установки дескриптора в GDT
void set_gdt_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// Временная функция для проверки
void test();

// Инициализация дескриптора и самой GDT
void init_gdt();

// Номера сегментов
extern uint32_t CODE_SEG;
extern uint32_t DATA_SEG;

#pragma once

#include <stdint.h>
#include "multiboot.h"
#include "fat.h"
#include "vbe.h"

#define MMAPADDR 0x5000

// Структура элемента карты памяти
typedef struct
{
    uint32_t base_low;
    uint32_t base_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
} MMapEntry;

// Получение размера нижней памяти через INT 0x12
uint16_t get_mem_lower();

// Получение размера верхней памяти через INT 0x15 (функция 0x88)
uint16_t get_mem_upper();

// Получение карты памяти
uint32_t fetch_mmap();

// Загрузка данных в структуру Multiboot
uint8_t fetch_multiboot(MultibootInfo* mb);

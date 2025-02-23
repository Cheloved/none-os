#pragma once

#include <stdint.h>

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

typedef struct
{
    uint32_t flags;                // Флаги доступных полей
    uint32_t mem_lower;            // Размер нижней памяти (в КБ)
    uint32_t mem_upper;            // Размер верхней памяти (в КБ)
    uint32_t boot_device;          // Устройство загрузки
    uint32_t cmdline;              // Командная строка ядра
    uint32_t mods_count;           // Количество загруженных модулей
    uint32_t mods_addr;            // Адрес списка модулей
    uint32_t syms[3];              // Информация о символах
    uint32_t mmap_length;          // Длина карты памяти
    uint32_t mmap_addr;            // Адрес карты памяти
    uint32_t drives_length;        // Длина информации о дисках
    uint32_t drives_addr;          // Адрес информации о дисках
    uint32_t config_table;         // Таблица конфигурации
    uint32_t boot_loader_name;     // Имя загрузчика
    uint32_t apm_table;            // Таблица APM
    uint32_t vbe_control_info;     // Информация о VBE
    uint32_t vbe_mode_info;        // Режим VBE
    uint16_t vbe_mode;             // Текущий режим VBE
    uint16_t vbe_interface_seg;    // Сегмент интерфейса VBE
    uint16_t vbe_interface_off;    // Смещение интерфейса VBE
    uint16_t vbe_interface_len;    // Длина интерфейса VBE

    uint32_t framebuffer_addr_low;
    uint32_t framebuffer_addr_high;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  color_info[6];

} MultibootInfo;

typedef struct
{
    uint16_t number;
    uint16_t width;
    uint16_t height;
    uint8_t  bpp;
    uint32_t* vmem;
} Info;

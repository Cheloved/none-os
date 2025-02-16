#pragma once

#include <stdint.h>
#include "stdio.h"

#define BOOT_BASE   0x7C00
#define FAT_BASE    0x7E00
#define KERNEL_BASE 0x10000000
#define MAXFILES    256

typedef struct
{
    uint8_t  ignore[3];
    uint8_t  oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint16_t dir_entries_count;
    uint16_t total_sectors;
    uint8_t  media_descriptor_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t  hidden_sectors;
    uint32_t  large_sector_count;

    uint8_t ebr_drive_number;
    uint8_t reserved;
    uint8_t ebr_signature;
    uint8_t ebr_volume_id[4];
    uint8_t ebr_volume_label[11];
    uint8_t ebr_system_id[8];
} __attribute__((packed)) BootData;

typedef struct
{
    uint8_t filename[8]; 
    uint8_t extension[3];
    uint8_t attributes;
    uint16_t reserved;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t ignore;
    uint16_t last_write_time;
    uint16_t last_write_date;
    uint16_t first_logical_cluster;
    uint32_t file_size;
} __attribute__((packed)) RootEntry;

// Структура DAP (Disk Address Packet)
typedef struct 
{
    uint16_t size;     // Размер DAP (0x10)
    uint16_t count;    // Количество секторов
    uint16_t offset;   // Смещение буфера
    uint16_t segment;  // Сегмент буфера
    uint32_t lba_low;  // Младшие 32 бита LBA
    uint32_t lba_high; // Старшие 32 бита (не используется)
} __attribute__((packed)) DAP;

extern uint16_t fat_size;
extern uint16_t root_dir_ptr;

extern BootData bd;
extern RootEntry files[MAXFILES];
extern uint16_t file_count;

void read_boot_data();
void read_root_dir();
uint8_t load_file(char filename[8], char extension[3], uint32_t base);

extern uint16_t __attribute__((cdecl)) _read_disk(uint32_t lba, uint8_t num_sectors, uint32_t buffer, uint8_t drive);
extern void __attribute__((cdecl)) kernel_jump();

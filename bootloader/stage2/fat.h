#pragma once

#include <stdint.h>

#define BOOT_BASE 0x7C00
#define FAT_BASE  0x7E00

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
    uint8_t  hidden_sectors;
    uint8_t  large_sector_count;

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

extern uint16_t fat_size;
extern uint16_t root_dir_ptr;

extern BootData bd;
extern RootEntry files[256];
extern uint16_t file_count;

void read_boot_data();
void read_root_dir();

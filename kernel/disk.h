#pragma once

#include <stdint.h>
#include "ports.h"
#include "stdio.h"

#define BOOTPTR     0x7C00
#define FATPTR      0x7E00
#define MAXFILES    256

// Порты контроллера IDE (Primary Channel)
#define IDE_DATA        0x1F0
#define IDE_SECTOR_CNT  0x1F2
#define IDE_LBA_LOW     0x1F3
#define IDE_LBA_MID     0x1F4
#define IDE_LBA_HIGH    0x1F5
#define IDE_DRIVE_SEL   0x1F6
#define IDE_COMMAND     0x1F7

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
extern RootEntry files[MAXFILES];
extern uint16_t file_count;

void read_boot_data();
void read_root_dir();
uint16_t get_file_first_cluster(char filename[8], char extension[3]);
/* uint8_t load_file(char filename[8], char extension[3], uint32_t base); */
void ata_init();
uint8_t ata_read(uint32_t lba, uint8_t* buffer);

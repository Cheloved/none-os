#include "fat.h"

uint16_t fat_size;
uint16_t root_dir_ptr;

BootData bd;
RootEntry files[256];
uint16_t file_count;

void read_boot_data()
{
    bd = *((BootData*)BOOT_BASE);
    fat_size = bd.fat_count * bd.sectors_per_fat * bd.bytes_per_sector;
    root_dir_ptr = (FAT_BASE + fat_size);
}

void read_root_dir()
{
    uint16_t ptr = (uint16_t)root_dir_ptr;
    file_count = 0;
    do
    {
        files[file_count++] = *((RootEntry*)ptr); 
        ptr += 32;
    } while ( files[file_count-1].first_logical_cluster != 0 &&
              file_count-1 < bd.dir_entries_count);
    file_count--;
}

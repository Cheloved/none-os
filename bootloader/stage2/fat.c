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

uint8_t load_file(char filename[8], char extension[3], uint32_t base)
{
    RootEntry* file = 0;

    for ( uint16_t i = 0; i < MAXFILES; i++ ) 
    {
        uint8_t found = 1;
        for ( uint8_t j = 0; j < 8; j++ )
            if ( filename[j] != files[i].filename[j] ) 
            { found = 0; break; }
        for ( uint8_t j = 0; j < 3; j++ )
            if ( extension[j] != files[i].extension[j] ) 
            { found = 0; break; }
        if ( !found )
            continue;

        file = &files[i];
    }

    if ( file == 0 )
        return 1;

    uint16_t cluster = file->first_logical_cluster;
    do
    {
        puts("Reading cluster "); putdec16(cluster); nl();
        uint16_t lba = 33 + (cluster-2) * bd.sectors_per_cluster;

        uint8_t err;
        if ( _read_disk(lba, bd.sectors_per_cluster, base, bd.ebr_drive_number) != 0 )
            { puts("Error in reading kernel\n\r"); return 2; }

        uint16_t offset = cluster * 3 / 2;
        uint16_t check = (cluster * 3) % 2;
        uint16_t next_cluster = *((uint16_t*)(FAT_BASE + offset));

        if ( check == 0 )
            cluster = next_cluster & 0x0FFF;
        else
            cluster = next_cluster >> 4;

        base += bd.bytes_per_sector * bd.sectors_per_cluster;

    } while (cluster < 0xFF8);

    return 0;
}


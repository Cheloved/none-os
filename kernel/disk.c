#include "disk.h"

uint16_t fat_size;
uint16_t root_dir_ptr;

BootData bd;
RootEntry files[256];
uint16_t file_count;

void read_boot_data()
{
    bd = *((BootData*)BOOTPTR);
    fat_size = bd.fat_count * bd.sectors_per_fat * bd.bytes_per_sector;
    root_dir_ptr = (FATPTR + fat_size);
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

uint16_t get_file_first_cluster(char filename[8], char extension[3])
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

    return file->first_logical_cluster;
}

void ata_init()
{
    outb(ATA_DRIVE_SEL, 0xE0 | (bd.ebr_drive_number << 4)); // LBA mode, Master
    while ((inb(ATA_COMMAND) & 0xC0) != 0x40);              // BUSY и DRDY биты

    if (inb(ATA_COMMAND) & 0x01) 
    {
        uint8_t error = inb(ATA_ERROR);
        puts(" [!] Error in ata_init(). Error code: ");
        puthex(error);
        nl();
    }
}

uint8_t ata_read(uint32_t lba, uint8_t* buffer)
{
    outb(ATA_SECTOR_CNT, 1);                          // Количество секторов
    outb(ATA_LBA_LOW, lba & 0xFF);                    // LBA Low
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);             // LBA Mid
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);           // LBA High
    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F)); // Drive и LBA High
    outb(ATA_COMMAND, 0x20);                          // Команда READ SECTORS

    // Ожидание готовности данных
    while ((inb(ATA_COMMAND) & 0x08) == 0);

    // Чтение данных
    for (int i = 0; i < 256; i++) 
    {
        uint16_t data = inw(ATA_DATA);
        buffer[i*2] = data & 0xFF;
        buffer[i*2 + 1] = (data >> 8) & 0xFF;
    }

    if (inb(ATA_COMMAND) & 0x01) 
    {
        uint8_t error = inb(ATA_ERROR);
        puts(" [!] Error in ata_read(). Error code: ");
        puthex(error);
        nl();
        return error;
    }

    return 0;
}

/* uint8_t load_file(char filename[8], char extension[3], uint32_t base) */
/* { */
/*     RootEntry* file = 0; */

/*     for ( uint16_t i = 0; i < MAXFILES; i++ ) */ 
/*     { */
/*         uint8_t found = 1; */
/*         for ( uint8_t j = 0; j < 8; j++ ) */
/*             if ( filename[j] != files[i].filename[j] ) */ 
/*             { found = 0; break; } */
/*         for ( uint8_t j = 0; j < 3; j++ ) */
/*             if ( extension[j] != files[i].extension[j] ) */ 
/*             { found = 0; break; } */
/*         if ( !found ) */
/*             continue; */

/*         file = &files[i]; */
/*     } */

/*     if ( file == 0 ) */
/*         return 1; */

/*     uint16_t cluster = file->first_logical_cluster; */
/*     do */
/*     { */
/*         puts("Reading cluster "); putdec(cluster); nl(); */
/*         uint16_t lba = 33 + (cluster-2) * bd.sectors_per_cluster; */

/*         uint8_t err; */
/*         if ( (err = read_disk(bd.ebr_drive_number, lba, bd.sectors_per_cluster, base)) != 0 ) */
/*             { puts("Err code: "); puthex(err); nl; return 2; } */

/*         uint16_t offset = cluster * 3 / 2; */
/*         uint16_t check = (cluster * 3) % 2; */
/*         uint16_t next_cluster = *((uint16_t*)(FATPTR + offset)); */

/*         if ( check == 0 ) */
/*             cluster = next_cluster & 0x0FFF; */
/*         else */
/*             cluster = next_cluster >> 4; */

/*         base += bd.bytes_per_sector * bd.sectors_per_cluster; */

/*     } while (cluster < 0xFF8); */

/*     return 0; */
/* } */

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

// Чтение сектора через PIO (Programmed I/O)
void ide_read_sector(uint32_t lba, uint8_t* buffer) {
    // Выбор диска (Master) и режим LBA
    outb(IDE_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(IDE_SECTOR_CNT, 1);        // Читаем 1 сектор
    outb(IDE_LBA_LOW, lba & 0xFF);  // Младшие биты LBA
    outb(IDE_LBA_MID, (lba >> 8) & 0xFF);
    outb(IDE_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(IDE_COMMAND, 0x20);        // Команда чтения

    // Ожидание готовности (BSY флаг сброшен, DRQ установлен)
    while ((inb(IDE_COMMAND) & 0x88) != 0x08);

    // Чтение данных из порта DATA (512 байт)
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(IDE_DATA);
        *((uint16_t*)buffer + i) = data;
    }
}

void ide_reset() 
{
    outb(0x3F6, 0x04);  // Установить бит SRST (Software Reset) в регистре управления
    for (volatile int i = 0; i < 1000; i++);  // Короткая задержка
    outb(0x3F6, 0x00);  // Сбросить SRST
    ide_wait_ready();    // Ожидание готовности
}

uint8_t ide_detect()
{
    outb(IDE_DRIVE_SEL, 0xA0);  // Выбор Master-устройства
    for (volatile int i = 0; i < 1000; i++);  // Задержка
    uint8_t status = inb(IDE_COMMAND);
    return (status != 0xFF);  // 0xFF означает отсутствие устройства
}

void ide_identify()
{
    outb(IDE_DRIVE_SEL, 0xA0);  // Выбор Master
    outb(IDE_COMMAND, 0xEC);     // Команда IDENTIFY

    ide_wait_ready();  // Ожидание готовности

    // Чтение 256 слов (512 байт) данных
    uint16_t buffer[256];
    for (int i = 0; i < 256; i++) {
        buffer[i] = inw(IDE_DATA);
    }

    // Проверка поддержки LBA (бит 9 в 49-м слове)
    if (buffer[49] & (1 << 9)) {
        // Устройство поддерживает LBA
    }
}

void ide_wait_ready()
{
    /* while ((inb(IDE_COMMAND) & 0x80)) {}  // Ждём сброса флага BSY (Busy) */
    /* while (!(inb(IDE_COMMAND) & 0x40)) {} // Ждём установки флага RDY (Ready) */
    uint8_t status;
    do {
        status = inb(IDE_COMMAND); // Читаем регистр статуса (0x1F7)
    } while ((status & 0x80) || !(status & 0x40)); // Ждём сброса BSY (0x80) и установки RDY (0x40)
}

void init_ide()
{
    ide_reset();          // Сброс контроллера
    if (!ide_detect())    // Проверка наличия диска
        puts("No IDE device detected\n");

    ide_identify();       // Получение информации об устройстве
}

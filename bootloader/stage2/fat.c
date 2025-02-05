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
        read_disk(bd.ebr_drive_number, lba, bd.sectors_per_cluster, base);

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

// Функция для чтения секторов с диска через int 0x13
int read_disk(uint8_t drive, uint16_t lba, uint8_t num_sectors, uint32_t buffer) {
    // Получение CHS адреса
    uint8_t sector;
    uint8_t cylinder;
    uint8_t head;
    lba_to_chs(lba, &cylinder, &head, &sector);

    uint16_t segment = buffer >> 16;     // Сегмент (старшие 16 бит)
    uint16_t offset = buffer & 0xFFFF;   // Смещение (младшие 16 бит)

    uint8_t error_code;
    __asm__ __volatile__(
        "pushw %%es\n"                    // Сохраняем текущее значение ES
        "movw %w3, %%ax\n"                // Загружаем сегмент в AX
        "int $0x13\n"                     // Вызов прерывания BIOS для работы с диском
        "setc %%al\n"                     // Установить AL = 1, если CF = 1 (ошибка)
        "movb %%ah, %0\n"                 // Сохранить код ошибки из AH в error_code
        "popw %%es\n"                     // Восстанавливаем предыдущее значение ES
        : "=a"(error_code)                // Возвращаемое значение: AL (флаг ошибки)
        : "a"(0x0200 | num_sectors),      // AH = 0x02 (чтение), AL = количество секторов
          "b"(offset),                    // BX = смещение
          "m"(segment),                   // segment = сегмент
          "c"((cylinder << 8) | sector),  // CH = цилиндр, CL = сектор
          "d"((head << 8) | drive)        // DH = головка, DL = номер диска
        : "memory", "cc"              // Информируем компилятор о побочных эффектах
    );

    // Проверка на ошибку
    if (error_code & 0x01) {
        return error_code;  // Вернуть код ошибки
    }

    return 0;  // Успех
}

void lba_to_chs(uint16_t lba, uint8_t* cylinder, uint8_t* head, uint8_t* sector)
{
    *sector   = lba % bd.sectors_per_track + 1;
    *cylinder = ( lba / bd.sectors_per_track ) / bd.heads;
    *head     = ( lba / bd.sectors_per_track ) % bd.heads;
}

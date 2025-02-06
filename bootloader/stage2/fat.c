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
        if ( (err = read_disk(bd.ebr_drive_number, lba, bd.sectors_per_cluster, base)) != 0 )
            { puts("Err code: "); puthex16(err); nl; return 2; }

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
uint16_t read_disk(uint8_t drive, uint16_t lba, uint8_t num_sectors, uint32_t buffer) {
    // Получение CHS адреса
    uint8_t sector;
    uint8_t cylinder;
    uint8_t head;
    lba_to_chs(lba, &cylinder, &head, &sector);

    uint16_t segment = buffer >> 16;     // Сегмент (старшие 16 бит)
    uint16_t offset = buffer & 0xFFFF;   // Смещение (младшие 16 бит)
    puts("Reading LBA "); putdec16(lba); puts(" to "); puthex16(segment); putc(':'); puthex16(offset);
    nl();

    uint8_t error_code;

    __asm__ __volatile__ (
        "pushw %%es\n\t"          // Сохраняем текущий сегмент ES
        "movw %w[seg], %%es\n\t"  // Устанавливаем сегмент ES
        "movb %[num_sectors], %%al\n\t"  // Количество секторов
        "movb %[cylinder], %%ch\n\t"     // Номер цилиндра
        "movb %[sector], %%cl\n\t"       // Номер сектора
        "movb %[head], %%dh\n\t"         // Номер головки
        "movb %[drive], %%dl\n\t"        // Номер диска
        "movw %w[offset], %%bx\n\t"      // Смещение буфера
        "movb $0x02, %%ah\n\t"           // Функция чтения секторов
        "int $0x13\n\t"                  // Вызов BIOS
        "jc .error\n\t"                  // Если CF = 1, произошла ошибка
        "xor %%ah, %%ah\n\t"             // Очищаем AH (успех)
        "jmp .done\n"
        ".error:\n\t"
        "movb %%ah, %[error]\n"          // Сохраняем код ошибки
        ".done:\n\t"
        "popw %%es\n\t"                  // Восстанавливаем сегмент ES
        : [error] "=m" (error_code)      // Выходной параметр: код ошибки
        : [num_sectors] "m" (num_sectors), [cylinder] "m" (cylinder),
          [sector] "m" (sector), [head] "m" (head), [drive] "m" (drive),
          [seg] "r" (segment), [offset] "r" (offset)
        : "eax", "ebx", "ecx", "edx", "memory", "cc"
    );

    return error_code;
}

void lba_to_chs(uint16_t lba, uint8_t* cylinder, uint8_t* head, uint8_t* sector)
{
    *sector   = lba % bd.sectors_per_track + 1;
    *cylinder = ( lba / bd.sectors_per_track ) / bd.heads;
    *head     = ( lba / bd.sectors_per_track ) % bd.heads;
}

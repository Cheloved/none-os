#include "fetching.h"

uint16_t get_mem_lower()
{
    uint16_t kb;
    asm volatile("int $0x12" : "=a"(kb));
    return kb;
}

uint16_t get_mem_upper()
{
    uint16_t kb;
    asm volatile("int $0x15" : "=a"(kb) : "a"(0x8800));
    return kb;
}

uint32_t fetch_mmap()
{
    MMapEntry* mmap = (MMapEntry*)MMAPADDR;
    uint32_t mmap_count = 0;
    uint32_t cont_id = 0;

    while (1) {
        uint32_t status;
        asm volatile(
            "int $0x15"
            : "=a"(status), "=b"(cont_id), "=c"(mmap[mmap_count].len_low)
            : "a"(0xE820), "b"(cont_id), "c"(24), "d"(0x534D4150), "D"(&mmap[mmap_count])
            : "memory", "cc"
        );
        if (status != 0x534D4150 || cont_id == 0) break;
        mmap_count++;
    }

}

uint8_t fetch_multiboot(MultibootInfo* mb)
{
    mb->flags = 0;

    // Заполнение mem_lower и mem_upper
    mb->mem_lower = get_mem_lower();
    mb->mem_upper = get_mem_upper();
    mb->flags |= 0x1; // Установка бита 0 

    // Номер загрузочного диска
    mb->boot_device = bd.ebr_drive_number;
    mb->flags |= 0x2;

    // cmdline, mods_{count,addr} и syms не используются

    // Карта памяти (INT 0x15, EAX=0xE820)
    uint32_t mmap_count = fetch_mmap();
    mb->mmap_addr = (uint32_t)MMAPADDR;
    mb->mmap_length = mmap_count * sizeof(MMapEntry);
    mb->flags |= 0x20;

    // dirves_{length,addr} и config_table не используются

    // Имя загрузчика
    mb->boot_loader_name = (uint32_t)("NONE");
    mb->flags |= 0x200;

    // apm_table и vbe_{control_info, mode_info} не используется

    mb->framebuffer_addr_low  = global_selected_vbemode.linear_buffer_addr;
    mb->framebuffer_addr_high = 0;
    mb->framebuffer_pitch     = 0;
    mb->framebuffer_width     = global_selected_vbemode.width;
    mb->framebuffer_height    = global_selected_vbemode.height;
    mb->framebuffer_bpp       = global_selected_vbemode.bits_per_pixel;
    mb->framebuffer_type      = global_selected_vbemode.mem_model_type;
    mb->flags |= 0x800;
}


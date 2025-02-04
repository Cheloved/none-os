#include "protected.h"

GDT_entry current_gdt[3];
GDT_descriptor current_gdt_desc;
uint32_t CODE_SEG = (uint32_t)&current_gdt[1] - (uint32_t)&current_gdt;
uint32_t DATA_SEG = (uint32_t)&current_gdt[2] - (uint32_t)&current_gdt;

void test()
{
    uint32_t color = 0x00ff0000;
    if ( !is_32bit_mode() )
        color = 0x0000ff00;
    uint32_t* buf = (uint32_t*)selected_mode.vmem;
    buf[0] = color;
    while(1);
}

int is_32bit_mode()
{
    int result = 0;
    __asm__ __volatile__(
        "pushf\n"                // Сохраняем флаги
        "pop %%eax\n"            // Загружаем флаги в EAX
        "mov %%eax, %%ebx\n"     // Копируем EAX в EBX
        "xor $0x200000, %%eax\n" // Инвертируем бит ID
        "push %%eax\n"           // Восстанавливаем изменённые флаги
        "popf\n"
        "pushf\n"                // Снова сохраняем флаги
        "pop %%eax\n"            // Загружаем флаги в EAX
        "cmp %%ebx, %%eax\n"     // Сравниваем исходные и изменённые флаги
        "setne %%al\n"           // Если они отличаются, значит CPUID доступен
        "movzx %%al, %0\n"       // Перемещаем результат в переменную
        : "=r" (result)
        :
        : "eax", "ebx", "memory"
    );
    return result;
}

void set_gdt_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    current_gdt[num].base_low    = (base & 0xFFFF);
    current_gdt[num].base_middle = (base >> 16) & 0xFF;
    current_gdt[num].base_high   = (base >> 24) & 0xFF;
    current_gdt[num].limit_low   = (limit & 0xFFFF);
    current_gdt[num].granularity = (limit >> 16) & 0x0F;
    current_gdt[num].granularity |= gran & 0xF0;
    current_gdt[num].access      = access;
}

void init_gdt()
{
    // Установка значений дескриптора
    current_gdt_desc.limit = (sizeof(GDT_entry) * 3) - 1;
    current_gdt_desc.base = (uint32_t)&current_gdt;

    set_gdt_entry(0, 0, 0, 0, 0);                // Null segment
    set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Кодовый сегмент
        // 0x9A = 0b11001111   // Present   = 1  - для используемых сегментов 
                               // Privilege = 00 - "ring"
                               // Type      = 1  - code/data
                               // Type flags:
                               //     1 - code
                               //     0 - conforming
                               //     1 - readable
                               //     0 - accessed ( устанавливается CPU )
        // 0xCF = 0b11001111,  // Other flags: 
                               //     1    - granularity ( limit *= 16 )
                               //     00   - для AVL (не используется) 
                               //     1111 - лимит (16-23)
                               // База (24)
    set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Сегмент данных
        // 0x92 - то же самое, но Type flags[0] = 0 - data

    // Загрукзка GDT
    asm volatile("lgdt %0" : : "m" (current_gdt_desc));

    // Включение защищенного режима
    asm volatile("mov %cr0, %eax; or %eax, 1; mov %eax, %cr0");

    // Переход к выполнению кода в защищенном режиме
    // TODO: заменить адрес на точку входа ядра
    __asm__ __volatile__
        (
            "mov %0, %%edi\n"
            "jmp *%%di\n"
            :
            : "r" (test)
            : "di", "memory"
        );
}

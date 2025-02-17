#include "interrupts.h"

InterruptDescriptor idt[IDT_ENTRIES];
IDT_ptr_t idt_ptr;

void idt_set_entry(int index, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt[index].offset_low = base & 0xFFFF;
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].type_attributes = flags;
    idt[index].offset_high = (base >> 16) & 0xFFFF;
}

void idt_init()
{
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_entry(i, (uint32_t)_default_int_handler, 0x08, 0x8E);

    /* idt_set_entry(0x21, (uint32_t)keyboard_handler, 0x08, 0x8E); */
    idt_set_entry(0x21, (uint32_t)_keyboard_int_handler, 0x08, 0x8E);

    asm volatile("lidt %0" : : "m"(idt_ptr));
}

void pic_remap(int offset1, int offset2)
{
    // Инициализация Master PIC
    outb(PIC1_COMMAND, 0x11); // Команда инициализации (ICW1)
    outb(PIC1_DATA, offset1); // Новый базовый вектор для Master PIC
    outb(PIC1_DATA, 0x04);    // Slave PIC подключен к IRQ2
    outb(PIC1_DATA, 0x01);    // Режим 8086

    // Инициализация Slave PIC
    outb(PIC2_COMMAND, 0x11);
    outb(PIC2_DATA, offset2); // Новый базовый вектор для Slave PIC
    outb(PIC2_DATA, 0x02);    // Slave PIC подключен к IRQ2 Master PIC
    outb(PIC2_DATA, 0x01);

    // Маскирование всех прерываний
    /* outb(PIC1_DATA, 0x00);    // Разрешить все прерывания на Master PIC */
    /* outb(PIC2_DATA, 0x00);    // Разрешить все прерывания на Slave PIC */
    // Разрешить только клавиатуру
    outb(0x21,0xfd);
    outb(0xa1,0xff);
}

void keyboard_handler()
{
    uint8_t scancode = inb(KEYBOARD_DATA_PORT); // Чтение скан-кода
    if (scancode <= 0x80) {                      // Проверка на нажатие клавиши
        switch (scancode) {
            case 0x02: putc('1'); break;
            case 0x03: putc('2'); break;
            case 0x04: putc('3'); break;
            default: break;
        }
    }
    // Отправка EOI (End of Interrupt) в PIC
    outb(0x20, 0x20); // Уведомление Master PIC
}


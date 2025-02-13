#include "ports.h"

void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"  // Инструкция outb: записать value в port
        :               // Нет выходных операндов
        : "a"(value),   // %0 = value (в регистре al/ax/eax)
          "Nd"(port)    // %1 = port (в регистре dx или непосредственное значение)
    );
}

// Чтение байта (8 бит) из порта ввода-вывода
uint8_t inb(uint16_t port)
{
    uint8_t result;
    __asm__ volatile (
        "inb %1, %0"  // Инструкция inb: прочитать из port в result
        : "=a"(result) // %0 = result (в регистре al/ax/eax)
        : "Nd"(port)   // %1 = port (в регистре dx или непосредственное значение)
    );
    return result;
}

// Чтение 2 байт (16 бит) из порта ввода-вывода
uint16_t inw(uint16_t port)
{
    uint16_t result;
    __asm__ volatile (
        "inw %1, %0"  // Инструкция inb: прочитать из port в result
        : "=a"(result) // %0 = result (в регистре al/ax/eax)
        : "Nd"(port)   // %1 = port (в регистре dx или непосредственное значение)
    );
    return result;
}

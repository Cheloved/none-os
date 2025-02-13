#pragma once
#include <stdint.h>

// Запись байта в порт ввода-вывода
void outb(uint16_t port, uint8_t value);

// Чтение байта из порта ввода-вывода
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

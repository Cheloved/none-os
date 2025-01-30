#pragma once
#include <stdint.h>

// Byte to ASCII
uint8_t btoa(uint8_t byte);
uint8_t atob(char c);
uint8_t sdec8tob(char* str);
uint16_t sdec16tob(char* str);

void putc(uint8_t c);
void puts(const char* str);
void putn(const char* str, uint16_t size);
void nl();
void puthex8(uint8_t hex);
void puthex16(uint16_t hex);
void putdec8(uint8_t dec);
void putdec16(uint16_t dec);

char getc();
void gets(char* buffer);

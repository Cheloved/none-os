#pragma once

#include <stdint.h>
#include "../bootloader/stage2/multiboot.h"

#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0x00FFFFFF
#define COLOR_RED   0x00FF0000
#define COLOR_GREEN 0x0000FF00
#define COLOR_BLUE  0x000000FF

#define LETTER_WIDTH 8
#define LETTER_HEIGHT 13

#define SPACING 2

extern uint32_t scrx;
extern uint32_t scry;
extern uint32_t color;

extern Info* inf;

uint8_t btoa(uint8_t byte);
uint8_t atob(char c);
void set_color(uint32_t color);
void clear_screen();
void putc(char c);
void puts(char* str);
void putn(char* str, uint32_t n);
void nl();
void puthex(uint32_t hex);
void putdec(uint32_t dec);

// 8x13 font
extern uint8_t bitmap[95][13];

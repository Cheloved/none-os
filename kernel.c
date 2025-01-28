#include <stdint.h>

uint32_t* fb;       // Адрес начала видеопамяти
uint16_t  width;    // Кол-во пикселей в ширину
uint16_t  height;   // Кол-во пикселей в ширину
uint8_t   bpp;      // Кол-во бит на пиксель

void fill_framebuffer(uint32_t* fb, uint32_t color, uint16_t width, uint16_t height) {
    uint32_t* end = fb + width * height;
    while (fb < end) {
        *fb++ = color;
    }
}

void main()
{
    fill_framebuffer(fb, (uint32_t)0x00000000, width, height);

    while(1);
}

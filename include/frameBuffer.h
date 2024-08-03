// frameBuffer.h
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

void framebuffer_init(void);
void put_pixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);
void draw_char(int x, int y, char c, uint32_t color);
void draw_string(int x, int y, const char* str, uint32_t color);
unsigned int get_width(void);
unsigned int get_height(void);

#endif // FRAMEBUFFER_H

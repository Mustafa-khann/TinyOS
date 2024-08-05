// frameBuffer.h
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

void framebuffer_init(void);
void clearScreen();
void put_pixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);
void draw_char(int x, int y, char c, uint32_t color);
void draw_string(int x, int y, const char* str, uint32_t color);
void draw_large_rotated_character(int x, int y, char c, uint32_t color, int scale);
void draw_large_rotated_string(int x, int y, const char* str, uint32_t color, int scale);
unsigned int get_width(void);
unsigned int get_height(void);
void delay(int seconds);


// For command line tool
void draw_cursor(int x, int y);
void clear_cursor(int x, int y);
void scroll_screen(void);

extern unsigned char *fb_global;
#endif // FRAMEBUFFER_H

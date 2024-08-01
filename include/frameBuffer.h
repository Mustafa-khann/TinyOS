#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

void framebuffer_init(void);
void put_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void draw_char(unsigned char ch, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void draw_string(const char *str, int x, int y, unsigned char r, unsigned char g, unsigned char b);

#endif

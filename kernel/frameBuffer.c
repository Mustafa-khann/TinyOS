#include "../include/frameBuffer.h"
#include "../include/mailbox.h"
#include "../include/font.h"
#include "../include/string.h"

static unsigned int width, height, pitch;
static unsigned char *fb;

void framebuffer_init() {
    unsigned int __attribute__((aligned(16))) mbox[35] = {
        35*4, 0, 0x48003,  // set phy wh
        8, 8, 1024, 768,   // wh
        0x48004,           // get phy wh
        8, 8, 0, 0,        // vars
        0x48009,           // get virt offs
        8, 8, 0, 0,        // vars
        0x48005,           // get depth
        4, 4, 32,          // 32bpp
        0x48006,           // get pixel order
        4, 4, 1,           // rgb
        0x40001,           // get framebuffer
        8, 8, 4096, 0,     // vars
        0x40008,           // get pitch
        4, 4, 0,           // vars
    };

    mailbox_write(8, (unsigned int)((unsigned long)mbox&~0xF));
    mailbox_read(8);

    if (mbox[20] == 32 && mbox[28] != 0) {
        width = mbox[5];
        height = mbox[6];
        pitch = mbox[33];
        fb = (unsigned char*)((unsigned long)mbox[28]);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            put_pixel(x, y, 0, 0, 64);  // Dark blue
        }
    }
}

void put_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    unsigned int offs = (y * pitch) + (x * 4);
    *((unsigned int*)(fb + offs)) = (r<<16) | (g<<8) | b;
}

void draw_char(unsigned char ch, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    const unsigned char *glyph = font + (ch * 16);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            if (glyph[i] & (1 << (7-j))) {
                put_pixel(x+j, y+i, r, g, b);
            }
        }
    }
}

void draw_string(const char *str, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    while (*str) {
        draw_char(*str++, x, y, r, g, b);
        x += 8;
    }
}

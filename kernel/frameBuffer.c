// frameBuffer.c
#include "../include/frameBuffer.h"
#include "../include/mailbox.h"
#include "../include/font.h"
#include "../include/uart.h"

static unsigned int width, height, pitch;
static unsigned char *fb;

void framebuffer_init(void) {
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

    // Debug output
    uart_puts("Framebuffer initialized:\n");
    uart_puts("Width: "); uart_puts(itoa(width, 10)); uart_puts("\n");
    uart_puts("Height: "); uart_puts(itoa(height, 10)); uart_puts("\n");
    uart_puts("Pitch: "); uart_puts(itoa(pitch, 10)); uart_puts("\n");
    uart_puts("Framebuffer address: "); uart_puts(itoa((unsigned int)fb[0], 16)); uart_puts("\n");
}

void put_pixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x < width && y < height) {
        unsigned int offs = (y * pitch) + (x * 4);
        *((unsigned int*)(fb + offs)) = (b << 16) | (g << 8) | r;  // RGB format
        __asm__ __volatile__ ("dsb sy");  // Data Synchronization Barrier
    }
}

void draw_char(int x, int y, char c, uint32_t color) {
    if (c < 32 || c > 127) return; // Unsupported characters

    const unsigned char * glyph = font[((unsigned char) c)-32];

    uart_puts("Drawing character '");
    uart_putc(c);
    uart_puts("' glyph:\n");

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (glyph[j] & (1 << i)) {
                unsigned char r = (color >> 16) & 0xFF;
                unsigned char g = (color >> 8) & 0xFF;
                unsigned char b = color & 0xFF;
                put_pixel(x + j, y + i, r, g, b);
                uart_putc('#');
            } else {
                uart_putc('.');
            }
        }
        uart_puts("\n");
    }

    uart_puts("Drew character '");
    uart_putc(c);
    uart_puts("' at (");
    uart_puts(itoa(x, 10));
    uart_puts(", ");
    uart_puts(itoa(y, 10));
    uart_puts(")\n");
}

void draw_string(int x, int y, const char* str, uint32_t color) {
    int offsetX = x;
    while (*str) {
        draw_char(offsetX, y, *str, color);
        offsetX += 8; // Move to the next character position
        str++;
    }

    // Debug output
    uart_puts("String drawn at (");
    uart_puts(itoa(x, 10));
    uart_puts(", ");
    uart_puts(itoa(y, 10));
    uart_puts("): ");
    uart_puts(str);
    uart_puts("\n");
}

unsigned int get_width(void) {
    return width;
}

unsigned int get_height(void) {
    return height;
}

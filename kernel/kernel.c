// kernel.c
#include "../include/frameBuffer.h"
#include "../include/uart.h"
#include "../include/font.h"
#include <stdbool.h>

static inline int abs(int value) {
    return value < 0 ? -value : value;
}

char* itoa(int value, int base) {
    static char buffer[33];
    char* ptr = &buffer[32];
    *ptr = '\0';

    if (value == 0) {
        *--ptr = '0';
    } else {
        unsigned int num = (value < 0) ? -value : value;
        while (num != 0) {
            *--ptr = "0123456789abcdef"[num % base];
            num /= base;
        }
        if (value < 0) {
            *--ptr = '-';
        }
    }

    return ptr;
}

void dump_memory(unsigned char* start, int length) {
    for (int i = 0; i < length; i++) {
        if (i % 16 == 0) {
            uart_puts("\n");
            uart_puts(itoa((unsigned int)*(start + i), 16));
            uart_puts(": ");
        }
        uart_puts(itoa(start[i], 16));
        uart_puts(" ");
    }
    uart_puts("\n");
}


void kernel_main(void) {

    volatile const unsigned char test_font[5][8] = {
        {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00},   // !
        {0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00},   // "
        {0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, 0x00, 0x00},   // #
        {0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00, 0x00, 0x00},   // $
        {0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00, 0x00},   // %
    };


    uart_init();
    uart_puts("Kernel started\n");

    // Print memory layout
    extern char __start, __end, __rodata_start, __rodata_end;
    uart_puts("Memory layout:\n");
    uart_puts("Kernel start: 0x"); uart_puts(itoa((unsigned int)&__start, 16)); uart_puts("\n");
    uart_puts("Rodata start: 0x"); uart_puts(itoa((unsigned int)&__rodata_start, 16)); uart_puts("\n");
    uart_puts("Rodata end:   0x"); uart_puts(itoa((unsigned int)&__rodata_end, 16)); uart_puts("\n");
    uart_puts("Kernel end:   0x"); uart_puts(itoa((unsigned int)&__end, 16)); uart_puts("\n");
    uart_puts("Font address: 0x"); uart_puts(itoa((unsigned int)font, 16)); uart_puts("\n");

    // Debug font data
    uart_puts("Test font data:\n");
    for (int i = 0; i < 5; i++) {
        uart_puts("Character ");
        uart_putc(i + 33);  // Start from '!'
        uart_puts(": ");
        for (int j = 0; j < 8; j++) {
            uart_puts(itoa(test_font[i][j], 16));
            uart_puts(" ");
        }
        uart_puts("\n");
    }

    for (int i = 0; i < 8; i++) {
        uart_puts(itoa(font['A' - 32][i], 16));
        uart_puts(" ");
    }
    uart_puts("\n");
    uart_puts("Kernel started\n");

    framebuffer_init();
    uart_puts("Framebuffer initialized\n");

    // Draw black background
    for (unsigned int y = 0; y < get_height(); y++) {
        for (unsigned int x = 0; x < get_width(); x++) {
            put_pixel(x, y, 0, 0, 0);  // Black
        }
    }
    uart_puts("Background drawn\n");

    // Draw colored rectangles for debugging
    for (int y = 0; y < 50; y++) {
        for (int x = 0; x < 50; x++) {
            put_pixel(x, y, 255, 0, 0);  // Red
            put_pixel(x + 60, y, 0, 255, 0);  // Green
            put_pixel(x + 120, y, 0, 0, 255);  // Blue
        }
    }
    uart_puts("Debug rectangles drawn\n");

    // Draw text
    uint32_t text_color = 0xFFFF00;  // Yellow color
    draw_string(100, 100, "Hello World!", text_color);
    uart_puts("Text drawn to framebuffer\n");

    // Simple delay
    for (volatile int i = 0; i < 10000000; i++) {}

    uart_puts("Kernel execution complete\n");

    uart_puts("Dumping font memory:\n");
    dump_memory((unsigned char*)font, 128);

    while(1) {}
}

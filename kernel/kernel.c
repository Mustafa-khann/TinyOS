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


// Function to draw a single character rotated 90 degrees clockwise and scaled up
void draw_large_rotated_character(int x, int y, char c, uint32_t color, int scale) {
    const unsigned char* char_bitmap = font[c - 32];
    for (int dy = 0; dy < 8; dy++) {
        for (int dx = 0; dx < 8; dx++) {
            if (char_bitmap[dy] & (1 << (7 - dx))) {
                // Draw a scaled block for each pixel, rotated 90 degrees clockwise
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        put_pixel(x + dy * scale + sy, y + (7 - dx) * scale + sx,
                                  (color >> 16) & 0xFF,
                                  (color >> 8) & 0xFF,
                                  color & 0xFF);
                    }
                }
            }
        }
    }
}

// Function to draw a string with each character rotated and scaled
void draw_large_rotated_string(int x, int y, const char* str, uint32_t color, int scale) {
    while (*str) {
        draw_large_rotated_character(x, y, *str, color, scale);
        x += (8 * scale) + scale; // Move to the right (character width + spacing)
        str++;
    }
}

void kernel_main(void) {
    uart_init();
    uart_puts("Kernel started\n");

    // Print memory layout (keeping this part unchanged)
    extern char __start, __end, __rodata_start, __rodata_end;
    uart_puts("Memory layout:\n");
    uart_puts("Kernel start: 0x"); uart_puts(itoa((unsigned int)&__start, 16)); uart_puts("\n");
    uart_puts("Rodata start: 0x"); uart_puts(itoa((unsigned int)&__rodata_start, 16)); uart_puts("\n");
    uart_puts("Rodata end:   0x"); uart_puts(itoa((unsigned int)&__rodata_end, 16)); uart_puts("\n");
    uart_puts("Kernel end:   0x"); uart_puts(itoa((unsigned int)&__end, 16)); uart_puts("\n");
    uart_puts("Font address: 0x"); uart_puts(itoa((unsigned int)font, 16)); uart_puts("\n");

    framebuffer_init();
    uart_puts("Framebuffer initialized\n");

    // Draw black background
    for (unsigned int y = 0; y < get_height(); y++) {
        for (unsigned int x = 0; x < get_width(); x++) {
            put_pixel(x, y, 0, 0, 0);  // Black
        }
    }
    uart_puts("Background drawn\n");

    // Calculate center position for text
    const char* welcome_text = "Welcome to TinyOS!";
    int text_length = 18; // Length of "Welcome to TinyOS!"
    int scale = 5; // Increase this value to make the font larger
    int char_width = 8 * scale;
    int char_height = 8 * scale;
    int char_spacing = scale;

    int text_width = text_length * (char_width + char_spacing) - char_spacing;
    int text_x = (get_width() - text_width) / 2;
    int text_y = (get_height() - char_width) / 2; // Use char_width as height due to rotation

    // Draw centered text with large rotated characters
    uint32_t text_color = 0xFFFFFF;  // White color
    draw_large_rotated_string(text_x, text_y, welcome_text, text_color, scale);
    uart_puts("Welcome text drawn to framebuffer\n");

    uart_puts("Kernel execution complete\n");

    while(1) {}
}

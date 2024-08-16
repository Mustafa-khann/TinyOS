#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

// Function prototypes
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags);

// UART functions
void uart_init(void);
void uart_putc(unsigned char c);
void uart_puts(const char* str);
unsigned char uart_getc(void);

// Framebuffer functions
void fb_init(void);
void fb_clear(uint32_t color);
void fb_put_pixel(int x, int y, uint32_t color);
void fb_draw_char(int x, int y, char c, uint32_t color);
void fb_draw_string(int x, int y, const char* str, uint32_t color);

// Utility functions
int strcmp(const char* s1, const char* s2);
void delay(int32_t count);

extern unsigned int get_system_clock(); // You'll need to implement this to get the system time
extern void get_memory_stats(unsigned int* total, unsigned int* used); // Implement this to get memory usage

#define TOTAL_MEMORY 1024 * 1024 * 128 // 128 MB, adjust as needed


#endif // KERNEL_H

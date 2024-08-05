// kernel.c
// header files
#include "../include/frameBuffer.h"
#include "../include/uart.h"
#include "../include/font.h"

// libraries
#include <stdbool.h>
#include <string.h>
// vars
#define MAX_CMD_LENGTH 256
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 30

char cmd_buffer[MAX_CMD_LENGTH];
int cmd_index = 0;
int cursor_x = 0, cursor_y = 0;

void process_command(void) {
    if (strcmp(cmd_buffer, "hello") == 0) {
        draw_string(0, (cursor_y + 1) * 16, "Hello, world!", 0xFFFFFF);
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        clearScreen();
        cursor_x = 0;
        cursor_y = 0;
    } else {
        char unknown_cmd[MAX_CMD_LENGTH + 16];
        strcpy(unknown_cmd, "Unknown command: ");
        strcat(unknown_cmd, cmd_buffer);
        draw_string(0, (cursor_y + 1) * 16, unknown_cmd, 0xFFFFFF);
    }
    cursor_y += 2;
    if (cursor_y >= SCREEN_HEIGHT) {
        scroll_screen();
        cursor_y = SCREEN_HEIGHT - 1;
    }
    cmd_index = 0;
    memset(cmd_buffer, 0, MAX_CMD_LENGTH);
}

void handle_keyboard_input(void) {
    int c = uart_getc_nb();
    if (c != -1) {
        if (c == '\r') {
            clear_cursor(cursor_x, cursor_y);
            cursor_x = 0;
            process_command();
            draw_string(cursor_x * 8, cursor_y * 16, "$ ", 0xFFFFFF);
            cursor_x = 2;
        } else if (c == '\b' && cmd_index > 0) {
            cmd_index--;
            clear_cursor(cursor_x, cursor_y);
            cursor_x--;
            draw_char(cursor_x * 8, cursor_y * 16, ' ', 0xFFFFFF);
        } else if (cmd_index < MAX_CMD_LENGTH - 1 && c >= 32 && c <= 126) {
            cmd_buffer[cmd_index++] = c;
            draw_char(cursor_x * 8, cursor_y * 16, c, 0xFFFFFF);
            cursor_x++;
        }
        if (cursor_x >= SCREEN_WIDTH) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= SCREEN_HEIGHT) {
                scroll_screen();
                cursor_y = SCREEN_HEIGHT - 1;
            }
        }
    }
}

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

    clearScreen();
    uart_puts("Screen Cleared\n");

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

    delay(500);

    clearScreen();
        draw_string(0, 0, "Welcome to TinyOS!", 0xFFFFFF);
        draw_string(0, 16, "$ ", 0xFFFFFF);
        cursor_x = 2;
        cursor_y = 1;

        while (1) {
            handle_keyboard_input();
            draw_cursor(cursor_x, cursor_y);
            delay(10); // Small delay to prevent busy-waiting
            clear_cursor(cursor_x, cursor_y);
        }
    }

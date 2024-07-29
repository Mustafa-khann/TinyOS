#include "kernel.h"

// Function to initialize UART
void uart_init() {
    mmio_write(UART0_CR, 0x00000000);
    mmio_write(UART0_ICR, 0x7FF);
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);
    mmio_write(UART0_LCRH, 0x70);
    mmio_write(UART0_CR, 0x301);
}

// Function to output a character to UART
void uart_putc(unsigned char c) {
    while (mmio_read(UART0_FR) & 0x20) {}
    mmio_write(UART0_DR, c);
}

// Function to output a string to UART
void uart_puts(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        uart_putc((unsigned char)str[i]);
    }
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
    (void)r0;
    (void)r1;
    (void)atags;

    uart_init();
    uart_puts("Hello from TinyOS!\n");
    uart_puts("Kernel initialized. Starting main loop...\n");

    int counter = 0;
    while (1) {
        // Turn on LED
        mmio_write(GPSET0, 1 << 16);
        for (int i = 0; i < 500000; i++) {} // Delay

        // Turn off LED
        mmio_write(GPCLR0, 1 << 16);
        for (int i = 0; i < 500000; i++) {} // Delay

        counter++;
        uart_puts("LED blink count: ");

        // Convert counter to string and output
        char count_str[20];
        int temp = counter;
        int i = 0;
        do {
            count_str[i++] = temp % 10 + '0';
            temp /= 10;
        } while (temp > 0);
        count_str[i] = '\0';

        // Output in reverse order
        for (int j = i - 1; j >= 0; j--) {
            uart_putc(count_str[j]);
        }
        uart_puts("\n");
    }
}

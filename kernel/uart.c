#include "../include/uart.h"

void uart_init(void) {
    mmio_write(UART0_CR, 0x00000000);
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 0x3);
    mmio_write(UART0_LCRH, 0x00000070);
    mmio_write(UART0_CR, 0x00000301);
}

void uart_putc(unsigned char c) {
    while (mmio_read(UART0_FR) & 0x20) {}
    mmio_write(UART0_DR, c);
}

void uart_puts(const char* str) {
    while (*str) {
        uart_putc(*str++);
    }
}

unsigned char uart_getc(void) {
    while (mmio_read(UART0_FR) & 0x10) {}
    return mmio_read(UART0_DR);
}

int uart_getc_nb(void) {
    if (mmio_read(UART0_FR) & 0x10) {
        return -1; // No data available
    }
    return mmio_read(UART0_DR);
}

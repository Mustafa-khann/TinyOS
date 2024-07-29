#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>

// Memory-Mapped I/O output
static inline void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)reg = data;
}

// Memory-Mapped I/O input
static inline uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t*)reg;
}

// GPIO
#define GPFSEL1         0x3F200004
#define GPSET0          0x3F20001C
#define GPCLR0          0x3F200028

// UART0
#define UART0_DR        0x3F201000
#define UART0_FR        0x3F201018
#define UART0_IBRD      0x3F201024
#define UART0_FBRD      0x3F201028
#define UART0_LCRH      0x3F20102C
#define UART0_CR        0x3F201030
#define UART0_IMSC      0x3F201038
#define UART0_ICR       0x3F201044

void uart_init();
void uart_putc(unsigned char c);
void uart_puts(const char* str);

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags);

#endif // KERNEL_H

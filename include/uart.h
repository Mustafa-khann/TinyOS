#ifndef UART_H
#define UART_H

#include <stdint.h>

// Memory-Mapped I/O output
static inline void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)reg = data;
}

// Memory-Mapped I/O input
static inline uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t*)reg;
}

// UART registers
#define UART0_BASE    0x3F201000
#define UART0_DR      (UART0_BASE + 0x00)
#define UART0_FR      (UART0_BASE + 0x18)
#define UART0_IBRD    (UART0_BASE + 0x24)
#define UART0_FBRD    (UART0_BASE + 0x28)
#define UART0_LCRH    (UART0_BASE + 0x2C)
#define UART0_CR      (UART0_BASE + 0x30)
#define UART0_IMSC    (UART0_BASE + 0x38)
#define UART0_ICR     (UART0_BASE + 0x44)

void uart_init(void);
void uart_putc(unsigned char c);
void uart_puts(const char* str);
unsigned char uart_getc(void);

#endif // UART_H

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "gpio.h"  // Include gpio.h for mmio_read and mmio_write definitions

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
int uart_getc_nb(void);

// Declare itoa as an external function
extern char* itoa(int value, int base);

#endif // UART_H

// gpio.h
#ifndef GPIO_H
#define GPIO_H

#define MMIO_BASE       0x3F000000

#define GPFSEL0         ((volatile unsigned int*)(MMIO_BASE+0x00200000))
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPFSEL2         ((volatile unsigned int*)(MMIO_BASE+0x00200008))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPPUDCLK1       ((volatile unsigned int*)(MMIO_BASE+0x0020009C))
#define GPEDS0          ((volatile unsigned int*)(MMIO_BASE+0x00200040))
#define GPFEN0          ((volatile unsigned int*)(MMIO_BASE+0x00200058))
#define GPLEV0          ((volatile unsigned int*)(MMIO_BASE+0x00200034))

#define IRQ_BASIC       ((volatile unsigned int*)(MMIO_BASE+0x0000B200))
#define IRQ_PEND1       ((volatile unsigned int*)(MMIO_BASE+0x0000B204))
#define IRQ_PEND2       ((volatile unsigned int*)(MMIO_BASE+0x0000B208))
#define IRQ_FIQ_CONTROL ((volatile unsigned int*)(MMIO_BASE+0x0000B20C))
#define IRQ_ENABLE_IRQS_1 ((volatile unsigned int*)(MMIO_BASE+0x0000B210))
#define IRQ_ENABLE_IRQS_2 ((volatile unsigned int*)(MMIO_BASE+0x0000B214))
#define IRQ_ENABLE_BASIC  ((volatile unsigned int*)(MMIO_BASE+0x0000B218))
#define IRQ_DISABLE_IRQS_1 ((volatile unsigned int*)(MMIO_BASE+0x0000B21C))
#define IRQ_DISABLE_IRQS_2 ((volatile unsigned int*)(MMIO_BASE+0x0000B220))
#define IRQ_DISABLE_BASIC  ((volatile unsigned int*)(MMIO_BASE+0x0000B224))

static inline void mmio_write(long reg, unsigned int val) { *(volatile unsigned int *)reg = val; }
static inline unsigned int mmio_read(long reg) { return *(volatile unsigned int *)reg; }

#endif // GPIO_H

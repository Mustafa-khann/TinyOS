// ps2Keyboard.c
#include "../include/gpio.h"
#include "../include/uart.h"
#include "../include/ps2Keyboard.h"

#define PS2_DATA    (1 << 19)  // GPIO pin 19 for data
#define PS2_CLK     (1 << 26)  // GPIO pin 26 for clock

static volatile unsigned char kbd_buffer[256];
static volatile unsigned int kbd_r_ptr = 0;
static volatile unsigned int kbd_w_ptr = 0;

void ps2Keyboard_init(void) {
    // Set GPIO pins as inputs
    mmio_write((unsigned long)GPFSEL1, mmio_read((unsigned long)GPFSEL1) & ~((7 << 27) | (7 << 24)));
    mmio_write((unsigned long)GPFSEL2, mmio_read((unsigned long)GPFSEL2) & ~(7 << 18));

    // Enable pull-up resistors
    mmio_write((unsigned long)GPPUD, 2);
    for(volatile int i = 0; i < 150; i++) { }
    mmio_write((unsigned long)GPPUDCLK0, PS2_DATA | PS2_CLK);
    for(volatile int i = 0; i < 150; i++) { }
    mmio_write((unsigned long)GPPUD, 0);
    mmio_write((unsigned long)GPPUDCLK0, 0);

    // Enable GPIO interrupts
    mmio_write((unsigned long)GPFEN0, PS2_CLK);
    mmio_write((unsigned long)IRQ_ENABLE_IRQS_2, (1 << 20));  // Enable GPIO interrupts
}

void ps2Keyboard_handler(void) {
    static unsigned int bit_count = 0;
    static unsigned char scancode = 0;
    static unsigned int clock_state = 0;

    unsigned int status = mmio_read((unsigned long)GPEDS0);

    if (status & PS2_CLK) {
        clock_state = mmio_read((unsigned long)GPLEV0) & PS2_CLK;

        if (clock_state == 0) {  // Falling edge
            if (bit_count < 8) {
                scancode |= ((mmio_read((unsigned long)GPLEV0) & PS2_DATA) ? 1 : 0) << bit_count;
                bit_count++;
            } else if (bit_count == 8) {
                // Parity bit, ignore for now
                bit_count++;
            } else {
                // Stop bit
                kbd_buffer[kbd_w_ptr] = scancode;
                kbd_w_ptr = (kbd_w_ptr + 1) % 256;
                bit_count = 0;
                scancode = 0;
            }
        }

        mmio_write((unsigned long)GPEDS0, PS2_CLK);  // Clear the interrupt
    }
}

int ps2Keyboard_getc(void) {
    if (kbd_r_ptr != kbd_w_ptr) {
        unsigned char c = kbd_buffer[kbd_r_ptr];
        kbd_r_ptr = (kbd_r_ptr + 1) % 256;
        return c;
    }
    return -1;
}

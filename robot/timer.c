#include "../include/timer.h"
#include "../include/gpio.h"

// BCM2835 system timer (free-running 1 MHz counter)
#define SYSTMR_BASE 0x3F003000
#define SYSTMR_CLO  (SYSTMR_BASE + 0x04)
#define SYSTMR_CHI  (SYSTMR_BASE + 0x08)

static int hw_ok = 0;
// Software fallback: callers of timer_sw_tick() advance this when no
// hardware counter is available.
static uint64_t sw_us = 0;

static uint64_t read_counter(void) {
    uint32_t hi, lo;
    do {
        hi = mmio_read(SYSTMR_CHI);
        lo = mmio_read(SYSTMR_CLO);
    } while (hi != mmio_read(SYSTMR_CHI));
    return ((uint64_t)hi << 32) | lo;
}

void timer_init(void) {
    uint32_t a = mmio_read(SYSTMR_CLO);
    for (volatile int i = 0; i < 1000; i++) { }
    hw_ok = (mmio_read(SYSTMR_CLO) != a);
}

int timer_hw_ok(void) {
    return hw_ok;
}

uint64_t timer_get_us(void) {
    if (hw_ok) return read_counter();
    // Coarse fallback: advance ~1us per call so periodic tasks still cycle.
    return ++sw_us;
}

uint32_t timer_get_ms(void) {
    return (uint32_t)(timer_get_us() / 1000);
}

void timer_delay_us(uint32_t us) {
    if (!hw_ok) {
        for (volatile uint32_t i = 0; i < us * 10; i++) { }
        sw_us += us;
        return;
    }
    uint64_t end = read_counter() + us;
    while (read_counter() < end) { }
}

void timer_delay_ms(uint32_t ms) {
    timer_delay_us(ms * 1000);
}

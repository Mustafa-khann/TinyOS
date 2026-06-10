// timer.h - monotonic time base for the robot stack.
// Backed by the BCM2835 free-running 1 MHz system timer (implemented by
// QEMU's raspi2b machine and present on real hardware). If the counter is
// not advancing (very old emulators), falls back to a coarse software tick.
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_init(void);
// Microseconds since boot, monotonic, 64-bit.
uint64_t timer_get_us(void);
uint32_t timer_get_ms(void);
// Busy-wait helpers built on the hardware counter.
void timer_delay_us(uint32_t us);
void timer_delay_ms(uint32_t ms);
// Nonzero if the hardware counter is alive (vs software fallback).
int timer_hw_ok(void);

#endif // TIMER_H

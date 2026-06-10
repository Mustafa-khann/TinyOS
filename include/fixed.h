// fixed.h - Q16.16 fixed-point math for the robot stack.
// The kernel runs without an FPU or soft-float runtime, so every quantity in
// the world model (meters, radians, m/s) is represented as Q16.16.
#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>

typedef int32_t fx_t;

#define FX_SHIFT 16
#define FX_ONE   ((fx_t)1 << FX_SHIFT)
#define FX_HALF  (FX_ONE >> 1)

// Common constants (Q16.16)
#define FX_PI      ((fx_t)205887)   // 3.14159265 * 65536
#define FX_TWO_PI  ((fx_t)411775)
#define FX_HALF_PI ((fx_t)102944)

#define FX_FROM_INT(x)   ((fx_t)(x) << FX_SHIFT)
#define FX_TO_INT(x)     ((fx_t)(x) >> FX_SHIFT)
// Build a constant from integer and thousandths, e.g. FX_MILLI(1, 500) == 1.5
#define FX_MILLI(i, m)   ((fx_t)((i) * 65536 + ((m) * 65536) / 1000))

static inline fx_t fx_mul(fx_t a, fx_t b) {
    return (fx_t)(((int64_t)a * (int64_t)b) >> FX_SHIFT);
}

static inline fx_t fx_div(fx_t a, fx_t b) {
    if (b == 0) return (a >= 0) ? INT32_MAX : INT32_MIN;
    return (fx_t)(((int64_t)a << FX_SHIFT) / b);
}

static inline fx_t fx_abs(fx_t a) {
    return (a < 0) ? -a : a;
}

static inline fx_t fx_clamp(fx_t v, fx_t lo, fx_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

fx_t fx_sin(fx_t radians);
fx_t fx_cos(fx_t radians);
fx_t fx_sqrt(fx_t a);
fx_t fx_atan2(fx_t y, fx_t x);
// Wrap an angle into (-pi, pi]
fx_t fx_wrap_angle(fx_t radians);
// Format a fixed-point value as "[-]i.mmm" (3 decimals) into buf, returns buf.
char* fx_to_str(fx_t v, char* buf);

#endif // FIXED_H

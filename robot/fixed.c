#include "../include/fixed.h"

// Quarter-wave sine table, sin(k * pi/2 / 64) in Q16.16, k = 0..64.
static const fx_t sin_lut[65] = {
    0, 1608, 3216, 4821, 6424, 8022, 9616, 11204,
    12785, 14359, 15924, 17479, 19024, 20557, 22078, 23586,
    25080, 26558, 28020, 29466, 30893, 32303, 33692, 35062,
    36410, 37736, 39040, 40320, 41576, 42806, 44011, 45190,
    46341, 47464, 48559, 49624, 50660, 51665, 52639, 53581,
    54491, 55368, 56212, 57022, 57798, 58538, 59244, 59914,
    60547, 61145, 61705, 62228, 62714, 63162, 63572, 63944,
    64277, 64571, 64827, 65043, 65220, 65358, 65457, 65516,
    65536,
};

fx_t fx_wrap_angle(fx_t a) {
    while (a > FX_PI)  a -= FX_TWO_PI;
    while (a <= -FX_PI) a += FX_TWO_PI;
    return a;
}

// Look up sin for an angle in [0, pi/2] with linear interpolation.
static fx_t sin_quarter(fx_t a) {
    // index = a / (pi/2) * 64; pi/2 in Q16.16 is FX_HALF_PI
    int64_t scaled = ((int64_t)a << 6); // a * 64
    int32_t idx = (int32_t)(scaled / FX_HALF_PI);
    if (idx >= 64) return FX_ONE;
    fx_t frac = (fx_t)((scaled % FX_HALF_PI) * FX_ONE / FX_HALF_PI);
    return sin_lut[idx] + fx_mul(sin_lut[idx + 1] - sin_lut[idx], frac);
}

fx_t fx_sin(fx_t a) {
    a = fx_wrap_angle(a);
    int neg = 0;
    if (a < 0) { a = -a; neg = 1; }
    if (a > FX_HALF_PI) a = FX_PI - a; // mirror into first quadrant
    fx_t s = sin_quarter(a);
    return neg ? -s : s;
}

fx_t fx_cos(fx_t a) {
    return fx_sin(a + FX_HALF_PI);
}

fx_t fx_sqrt(fx_t a) {
    if (a <= 0) return 0;
    // sqrt of Q16.16 is isqrt(value << 16)
    uint64_t n = ((uint64_t)(uint32_t)a) << FX_SHIFT;
    uint64_t x = n;
    uint64_t res = 0;
    uint64_t bit = (uint64_t)1 << 46; // highest even bit below 48
    while (bit > x) bit >>= 2;
    while (bit != 0) {
        if (x >= res + bit) {
            x -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return (fx_t)res;
}

// atan on [0,1] via polynomial: atan(z) ~= z*(pi/4 + 0.273*(1 - z))
static fx_t atan_unit(fx_t z) {
    static const fx_t QTR_PI = 51472;  // pi/4
    static const fx_t K = 17891;       // 0.273
    return fx_mul(z, QTR_PI + fx_mul(K, FX_ONE - z));
}

fx_t fx_atan2(fx_t y, fx_t x) {
    if (x == 0 && y == 0) return 0;
    fx_t ax = fx_abs(x), ay = fx_abs(y);
    fx_t angle;
    if (ax >= ay) {
        angle = atan_unit(fx_div(ay, ax));
    } else {
        angle = FX_HALF_PI - atan_unit(fx_div(ax, ay));
    }
    if (x < 0) angle = FX_PI - angle;
    if (y < 0) angle = -angle;
    return angle;
}

char* fx_to_str(fx_t v, char* buf) {
    char* p = buf;
    uint32_t uv;
    if (v < 0) {
        *p++ = '-';
        uv = (uint32_t)(-(int64_t)v);
    } else {
        uv = (uint32_t)v;
    }
    uint32_t ip = uv >> FX_SHIFT;
    // 3 decimal digits, rounded
    uint32_t frac = (uint32_t)((((uint64_t)(uv & 0xFFFF)) * 1000 + 32768) >> FX_SHIFT);
    if (frac >= 1000) { ip++; frac -= 1000; }

    // integer part
    char tmp[12];
    int n = 0;
    do {
        tmp[n++] = '0' + (ip % 10);
        ip /= 10;
    } while (ip);
    while (n) *p++ = tmp[--n];

    *p++ = '.';
    *p++ = '0' + (frac / 100) % 10;
    *p++ = '0' + (frac / 10) % 10;
    *p++ = '0' + frac % 10;
    *p = '\0';
    return buf;
}

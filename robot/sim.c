#include "../include/sim.h"

#define ROBOT_RADIUS FX_MILLI(0, 150)   // 0.15 m
#define ACT_TAU      FX_MILLI(0, 200)   // actuator lag time constant, s
#define V_MAX        FX_MILLI(0, 500)   // 0.5 m/s
#define W_MAX        FX_MILLI(1, 800)   // 1.8 rad/s

static sim_state_t st;
static sim_obstacle_t obstacles[SIM_MAX_OBSTACLES];
static int n_obstacles;
static uint32_t rng = 0x12345678;

void sim_init(void) {
    st.x = 0; st.y = 0; st.theta = 0;
    st.v = 0; st.w = 0;
    st.cmd_v = 0; st.cmd_w = 0;
    st.collisions = 0;

    n_obstacles = 0;
    obstacles[n_obstacles++] = (sim_obstacle_t){ FX_FROM_INT(2), FX_MILLI(0, 600), FX_MILLI(0, 350) };
    obstacles[n_obstacles++] = (sim_obstacle_t){ FX_MILLI(1, 500), -FX_ONE, FX_MILLI(0, 400) };
    obstacles[n_obstacles++] = (sim_obstacle_t){ FX_FROM_INT(3), FX_MILLI(1, 600), FX_MILLI(0, 500) };
    obstacles[n_obstacles++] = (sim_obstacle_t){ FX_MILLI(3, 500), -FX_MILLI(0, 800), FX_MILLI(0, 300) };
}

uint32_t sim_rand(void) {
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}

fx_t sim_noise(fx_t amp) {
    // Uniform in [-amp, amp]
    int32_t r = (int32_t)(sim_rand() & 0xFFFF) - 0x8000; // [-32768, 32767]
    return (fx_t)(((int64_t)r * amp) >> 15);
}

void sim_set_cmd(fx_t v, fx_t w) {
    st.cmd_v = fx_clamp(v, -V_MAX, V_MAX);
    st.cmd_w = fx_clamp(w, -W_MAX, W_MAX);
}

static int hits_obstacle(fx_t x, fx_t y) {
    for (int i = 0; i < n_obstacles; i++) {
        fx_t dx = x - obstacles[i].x;
        fx_t dy = y - obstacles[i].y;
        fx_t lim = obstacles[i].r + ROBOT_RADIUS;
        // Compare squared distances to avoid sqrt.
        if (fx_mul(dx, dx) + fx_mul(dy, dy) < fx_mul(lim, lim)) return 1;
    }
    return 0;
}

void sim_step(fx_t dt) {
    // First-order actuator lag toward the commanded velocities.
    fx_t alpha = fx_div(dt, ACT_TAU + dt);
    st.v += fx_mul(st.cmd_v - st.v, alpha);
    st.w += fx_mul(st.cmd_w - st.w, alpha);

    // Unicycle integration with wheel-slip noise proportional to motion.
    fx_t v = st.v + fx_mul(fx_abs(st.v), sim_noise(FX_MILLI(0, 30)));
    fx_t w = st.w + fx_mul(fx_abs(st.w) + fx_abs(st.v), sim_noise(FX_MILLI(0, 40)));
    fx_t nx = st.x + fx_mul(fx_mul(v, fx_cos(st.theta)), dt);
    fx_t ny = st.y + fx_mul(fx_mul(v, fx_sin(st.theta)), dt);

    if (hits_obstacle(nx, ny)) {
        // Bump: kill linear motion, rotation still allowed.
        st.v = 0;
        st.collisions++;
    } else {
        st.x = nx;
        st.y = ny;
    }
    st.theta = fx_wrap_angle(st.theta + fx_mul(w, dt));
}

const sim_state_t* sim_truth(void) {
    return &st;
}

int sim_obstacle_count(void) {
    return n_obstacles;
}

const sim_obstacle_t* sim_obstacle(int i) {
    if (i < 0 || i >= n_obstacles) return 0;
    return &obstacles[i];
}

fx_t sim_raycast(fx_t bearing, fx_t max_range) {
    // March the ray in 5 cm steps; fine for a toy world.
    const fx_t step = FX_MILLI(0, 50);
    fx_t cx = fx_cos(bearing), cy = fx_sin(bearing);
    for (fx_t d = step; d <= max_range; d += step) {
        fx_t px = st.x + fx_mul(cx, d);
        fx_t py = st.y + fx_mul(cy, d);
        for (int i = 0; i < n_obstacles; i++) {
            fx_t dx = px - obstacles[i].x;
            fx_t dy = py - obstacles[i].y;
            if (fx_mul(dx, dx) + fx_mul(dy, dy) < fx_mul(obstacles[i].r, obstacles[i].r)) {
                return d;
            }
        }
    }
    return max_range;
}

// sim.h - built-in ground-truth simulator (QEMU backend for the HAL).
// Integrates a differential-drive robot in a small 2D world with circular
// obstacles. The HAL sensor drivers sample this ground truth with noise;
// the motor driver feeds commands into it. The world model never reads the
// simulator directly - only through the (noisy) HAL, exactly as it would
// on real hardware.
#ifndef SIM_H
#define SIM_H

#include "fixed.h"
#include <stdint.h>

#define SIM_MAX_OBSTACLES 8

typedef struct {
    fx_t x, y, r;
} sim_obstacle_t;

typedef struct {
    fx_t x, y, theta;     // true pose
    fx_t v, w;            // true velocities
    fx_t cmd_v, cmd_w;    // last commanded velocities
    uint32_t collisions;  // times the robot bumped an obstacle
} sim_state_t;

void sim_init(void);
// Advance ground truth by dt (Q16.16 seconds).
void sim_step(fx_t dt);
void sim_set_cmd(fx_t v, fx_t w);
const sim_state_t* sim_truth(void);
int sim_obstacle_count(void);
const sim_obstacle_t* sim_obstacle(int i);
// Distance from true pose along absolute bearing until an obstacle or
// max_range; used by the simulated rangefinder.
fx_t sim_raycast(fx_t bearing, fx_t max_range);

// Simple PRNG shared by the sensor noise models.
uint32_t sim_rand(void);
// Uniform noise in [-amp, amp] (Q16.16).
fx_t sim_noise(fx_t amp);

#endif // SIM_H

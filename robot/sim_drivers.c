// sim_drivers.c - HAL devices backed by the built-in simulator.
// Each driver mimics a real part: an IMU with gyro bias, quadrature wheel
// encoders with measurement noise, a 5-ray rangefinder, and a differential
// drive motor controller. Swap this file for real I2C/PWM drivers on
// hardware; the topic bus and world model are unchanged.
#include "../include/hal.h"
#include "../include/sim.h"
#include "../include/robot_msgs.h"
#include "../include/timer.h"

#define WHEEL_TRACK FX_MILLI(0, 300)   // 0.30 m between wheels
#define GYRO_BIAS   FX_MILLI(0, 8)     // 0.008 rad/s constant bias

static int imu_read(void* buf, int len) {
    if (len < (int)sizeof(msg_imu_t)) return -1;
    const sim_state_t* t = sim_truth();
    msg_imu_t* m = (msg_imu_t*)buf;
    m->gyro_z = t->w + GYRO_BIAS + sim_noise(FX_MILLI(0, 15));
    m->accel_x = sim_noise(FX_MILLI(0, 50));
    m->accel_y = fx_mul(t->v, t->w) + sim_noise(FX_MILLI(0, 50)); // centripetal
    m->stamp_us = timer_get_us();
    return sizeof(msg_imu_t);
}

static int encoders_read(void* buf, int len) {
    if (len < (int)sizeof(msg_encoders_t)) return -1;
    const sim_state_t* t = sim_truth();
    msg_encoders_t* m = (msg_encoders_t*)buf;
    fx_t half_track = WHEEL_TRACK >> 1;
    m->v_left  = t->v - fx_mul(t->w, half_track) + sim_noise(FX_MILLI(0, 8));
    m->v_right = t->v + fx_mul(t->w, half_track) + sim_noise(FX_MILLI(0, 8));
    m->stamp_us = timer_get_us();
    return sizeof(msg_encoders_t);
}

static int range_read(void* buf, int len) {
    if (len < (int)sizeof(msg_range_t)) return -1;
    const sim_state_t* t = sim_truth();
    msg_range_t* m = (msg_range_t*)buf;
    for (int i = 0; i < RANGE_NUM_RAYS; i++) {
        fx_t bearing = fx_wrap_angle(t->theta + RANGE_BEARINGS[i]);
        fx_t d = sim_raycast(bearing, RANGE_MAX);
        if (d < RANGE_MAX) d += sim_noise(FX_MILLI(0, 20));
        m->range[i] = d;
    }
    m->stamp_us = timer_get_us();
    return sizeof(msg_range_t);
}

static int motor_write(const void* buf, int len) {
    if (len < (int)sizeof(msg_cmd_vel_t)) return -1;
    const msg_cmd_vel_t* m = (const msg_cmd_vel_t*)buf;
    sim_set_cmd(m->v, m->w);
    return len;
}

void sim_drivers_register(void) {
    hal_register("imu0",   HAL_SENSOR,   imu_read,      0);
    hal_register("enc0",   HAL_SENSOR,   encoders_read, 0);
    hal_register("range0", HAL_SENSOR,   range_read,    0);
    hal_register("motor0", HAL_ACTUATOR, 0,             motor_write);
}

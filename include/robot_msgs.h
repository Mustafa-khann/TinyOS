// robot_msgs.h - message types carried on the topic bus.
// Units: meters, radians, seconds (velocities m/s, rad/s), all Q16.16.
#ifndef ROBOT_MSGS_H
#define ROBOT_MSGS_H

#include <stdint.h>
#include "fixed.h"

#define RANGE_NUM_RAYS 5

// Topic names
#define TOPIC_IMU     "imu"
#define TOPIC_ODOM    "odom"
#define TOPIC_RANGE   "range"
#define TOPIC_CMD_VEL "cmd_vel"
#define TOPIC_GOAL    "goal"

typedef struct {
    fx_t gyro_z;        // yaw rate, rad/s
    fx_t accel_x;       // body-frame, m/s^2
    fx_t accel_y;
    uint64_t stamp_us;
} msg_imu_t;

typedef struct {
    fx_t v_left;        // left wheel ground speed, m/s
    fx_t v_right;
    uint64_t stamp_us;
} msg_encoders_t;

typedef struct {
    // Rays at fixed bearings relative to robot heading; see RANGE_BEARINGS.
    fx_t range[RANGE_NUM_RAYS];   // meters, RANGE_MAX if no hit
    uint64_t stamp_us;
} msg_range_t;

typedef struct {
    fx_t v;             // commanded linear velocity, m/s
    fx_t w;             // commanded angular velocity, rad/s
    uint64_t stamp_us;
} msg_cmd_vel_t;

typedef struct {
    fx_t x;
    fx_t y;
    uint8_t active;
    uint64_t stamp_us;
} msg_goal_t;

#define RANGE_MAX FX_FROM_INT(4)   // 4.0 m sensor ceiling

// Bearings of the 5 range rays (radians, Q16.16): -60, -30, 0, +30, +60 deg.
static const fx_t RANGE_BEARINGS[RANGE_NUM_RAYS] = {
    -68625, -34312, 0, 34312, 68625
};

#endif // ROBOT_MSGS_H

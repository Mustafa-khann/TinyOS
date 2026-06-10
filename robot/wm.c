#include "../include/wm.h"
#include "../include/topic.h"
#include "../include/timer.h"
#include "../include/string.h"

#define WHEEL_TRACK FX_MILLI(0, 300)

// Filter gains (complementary-filter style correction).
#define K_VEL    FX_MILLI(0, 400)  // trust in encoder velocity per update
#define K_GYRO   FX_MILLI(0, 700)  // gyro weight for yaw rate blend
// Uncertainty model: grows with distance travelled / rotation, shrinks on
// measurement updates.
#define Q_POS    FX_MILLI(0, 30)   // sigma growth per meter travelled
#define Q_THETA  FX_MILLI(0, 60)   // sigma growth per radian turned
#define R_SHRINK FX_MILLI(0, 990)  // sigma retain factor on measurement

#define OBST_PT_TTL_US  4000000ULL // forget returns older than 4 s

static wm_state_t belief;
static wm_obst_pt_t obst[WM_MAX_OBST_PTS];
static int obst_head = 0;

static topic_t* t_imu;
static topic_t* t_odom;
static topic_t* t_range;
static uint32_t cur_imu, cur_odom, cur_range;

void wm_init(void) {
    memset(&belief, 0, sizeof(belief));
    memset(obst, 0, sizeof(obst));
    obst_head = 0;
    belief.sigma_pos = FX_MILLI(0, 10);
    belief.sigma_theta = FX_MILLI(0, 10);
    belief.stamp_us = timer_get_us();

    t_imu = topic_advertise(TOPIC_IMU, sizeof(msg_imu_t));
    t_odom = topic_advertise(TOPIC_ODOM, sizeof(msg_encoders_t));
    t_range = topic_advertise(TOPIC_RANGE, sizeof(msg_range_t));
    cur_imu = cur_odom = cur_range = 0;
}

void wm_reset_pose(fx_t x, fx_t y, fx_t theta) {
    belief.x = x;
    belief.y = y;
    belief.theta = theta;
    belief.sigma_pos = FX_MILLI(0, 10);
    belief.sigma_theta = FX_MILLI(0, 10);
}

void wm_predict(const wm_state_t* from, fx_t v, fx_t w, fx_t dt, wm_state_t* out) {
    *out = *from;
    out->x = from->x + fx_mul(fx_mul(v, fx_cos(from->theta)), dt);
    out->y = from->y + fx_mul(fx_mul(v, fx_sin(from->theta)), dt);
    out->theta = fx_wrap_angle(from->theta + fx_mul(w, dt));
    out->v = v;
    out->w = w;
    // Uncertainty grows with motion.
    out->sigma_pos = from->sigma_pos + fx_mul(Q_POS, fx_mul(fx_abs(v), dt));
    out->sigma_theta = from->sigma_theta + fx_mul(Q_THETA, fx_mul(fx_abs(w), dt));
}

static void remember_obstacles(const msg_range_t* r) {
    for (int i = 0; i < RANGE_NUM_RAYS; i++) {
        if (r->range[i] >= RANGE_MAX) continue;
        fx_t bearing = fx_wrap_angle(belief.theta + RANGE_BEARINGS[i]);
        wm_obst_pt_t* p = &obst[obst_head];
        p->x = belief.x + fx_mul(fx_cos(bearing), r->range[i]);
        p->y = belief.y + fx_mul(fx_sin(bearing), r->range[i]);
        p->stamp_us = r->stamp_us;
        obst_head = (obst_head + 1) % WM_MAX_OBST_PTS;
    }
}

void wm_update_task(void) {
    uint64_t now = timer_get_us();
    fx_t dt = (fx_t)(((now - belief.stamp_us) << FX_SHIFT) / 1000000);
    if (dt <= 0) return;
    if (dt > FX_MILLI(0, 100)) dt = FX_MILLI(0, 100); // clamp after stalls

    // 1. Predict: propagate belief through the dynamics model.
    wm_state_t pred;
    wm_predict(&belief, belief.v, belief.w, dt, &pred);
    belief = pred;
    belief.stamp_us = now;

    // 2. Correct from encoder odometry.
    msg_encoders_t enc;
    if (topic_poll(t_odom, &cur_odom, &enc)) {
        fx_t v_meas = (enc.v_left + enc.v_right) >> 1;
        fx_t w_meas = fx_div(enc.v_right - enc.v_left, WHEEL_TRACK);
        belief.v += fx_mul(v_meas - belief.v, K_VEL);
        belief.w += fx_mul(w_meas - belief.w, FX_ONE - K_GYRO);
        belief.sigma_pos = fx_mul(belief.sigma_pos, R_SHRINK);
    }

    // 3. Correct yaw rate from the gyro (dominant weight).
    msg_imu_t imu;
    if (topic_poll(t_imu, &cur_imu, &imu)) {
        belief.w += fx_mul(imu.gyro_z - belief.w, K_GYRO);
        belief.sigma_theta = fx_mul(belief.sigma_theta, R_SHRINK);
    }

    // 4. Fold range returns into obstacle memory.
    msg_range_t rng;
    if (topic_poll(t_range, &cur_range, &rng)) {
        remember_obstacles(&rng);
    }
}

void wm_get(wm_state_t* out) {
    *out = belief;
}

int wm_obstacle_points(const wm_obst_pt_t** out) {
    *out = obst;
    return WM_MAX_OBST_PTS;
}

fx_t wm_clearance(fx_t x, fx_t y) {
    uint64_t now = timer_get_us();
    fx_t best = FX_FROM_INT(100);
    for (int i = 0; i < WM_MAX_OBST_PTS; i++) {
        if (obst[i].stamp_us == 0) continue;
        if (now - obst[i].stamp_us > OBST_PT_TTL_US) continue;
        fx_t dx = x - obst[i].x;
        fx_t dy = y - obst[i].y;
        fx_t d2 = fx_mul(dx, dx) + fx_mul(dy, dy);
        fx_t d = fx_sqrt(d2);
        if (d < best) best = d;
    }
    return best;
}

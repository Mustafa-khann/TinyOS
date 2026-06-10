#include "../include/planner.h"
#include "../include/wm.h"
#include "../include/topic.h"
#include "../include/timer.h"
#include "../include/robot_msgs.h"

#define HORIZON_STEPS   8
#define STEP_DT         FX_MILLI(0, 150)  // 150 ms per imagined step
#define ARRIVE_DIST     FX_MILLI(0, 200)  // 20 cm counts as arrived
#define MIN_CLEARANCE   FX_MILLI(0, 280)  // keep 28 cm from obstacle returns
#define BIG_PENALTY     FX_FROM_INT(1000)

static const fx_t V_CANDS[] = {
    0, FX_MILLI(0, 120), FX_MILLI(0, 250), FX_MILLI(0, 400)
};
static const fx_t W_CANDS[] = {
    -FX_MILLI(1, 400), -FX_MILLI(0, 700), -FX_MILLI(0, 300),
    0,
    FX_MILLI(0, 300), FX_MILLI(0, 700), FX_MILLI(1, 400)
};
#define NV ((int)(sizeof(V_CANDS) / sizeof(V_CANDS[0])))
#define NW ((int)(sizeof(W_CANDS) / sizeof(W_CANDS[0])))

static plan_status_t status = PLAN_IDLE;
static fx_t goal_x, goal_y;
static fx_t best_v, best_w;
static topic_t* t_cmd;

void planner_init(void) {
    status = PLAN_IDLE;
    goal_x = goal_y = 0;
    best_v = best_w = 0;
    t_cmd = topic_advertise(TOPIC_CMD_VEL, sizeof(msg_cmd_vel_t));
}

void planner_set_goal(fx_t x, fx_t y) {
    goal_x = x;
    goal_y = y;
    status = PLAN_SEEKING;
}

void planner_stop(void) {
    status = PLAN_IDLE;
    best_v = best_w = 0;
    msg_cmd_vel_t cmd = { 0, 0, timer_get_us() };
    topic_publish(t_cmd, &cmd);
}

plan_status_t planner_status(void) { return status; }
void planner_goal(fx_t* x, fx_t* y) { *x = goal_x; *y = goal_y; }
fx_t planner_last_best_v(void) { return best_v; }
fx_t planner_last_best_w(void) { return best_w; }

static fx_t dist_to_goal(fx_t x, fx_t y) {
    fx_t dx = goal_x - x, dy = goal_y - y;
    return fx_sqrt(fx_mul(dx, dx) + fx_mul(dy, dy));
}

// Roll (v, w) forward through the world model and score the imagined
// trajectory: lower is better.
static fx_t rollout_cost(const wm_state_t* start, fx_t v, fx_t w, int* collided) {
    wm_state_t s = *start;
    *collided = 0;
    fx_t min_clear = FX_FROM_INT(100);

    for (int k = 0; k < HORIZON_STEPS; k++) {
        wm_state_t next;
        wm_predict(&s, v, w, STEP_DT, &next);
        s = next;
        fx_t c = wm_clearance(s.x, s.y);
        if (c < min_clear) min_clear = c;
        if (c < MIN_CLEARANCE) {
            *collided = 1;
            return BIG_PENALTY + dist_to_goal(s.x, s.y);
        }
    }

    fx_t cost = dist_to_goal(s.x, s.y);
    // Prefer trajectories that face the goal at the end.
    fx_t want = fx_atan2(goal_y - s.y, goal_x - s.x);
    cost += fx_abs(fx_wrap_angle(want - s.theta)) >> 3;
    // Mild preference for going straight and keeping margin from obstacles.
    cost += fx_abs(w) >> 4;
    if (min_clear < FX_MILLI(0, 600)) {
        cost += (FX_MILLI(0, 600) - min_clear) >> 1;
    }
    return cost;
}

void planner_task(void) {
    if (status != PLAN_SEEKING && status != PLAN_STUCK) return;

    wm_state_t now;
    wm_get(&now);

    if (dist_to_goal(now.x, now.y) < ARRIVE_DIST) {
        status = PLAN_ARRIVED;
        best_v = best_w = 0;
        msg_cmd_vel_t cmd = { 0, 0, timer_get_us() };
        topic_publish(t_cmd, &cmd);
        return;
    }

    fx_t best_cost = BIG_PENALTY * 2;
    fx_t bv = 0, bw = 0;
    int all_collide = 1;

    for (int i = 0; i < NV; i++) {
        for (int j = 0; j < NW; j++) {
            // Standing still and not turning gets us nowhere.
            if (V_CANDS[i] == 0 && W_CANDS[j] == 0) continue;
            int collided;
            fx_t c = rollout_cost(&now, V_CANDS[i], W_CANDS[j], &collided);
            if (!collided) all_collide = 0;
            if (c < best_cost) {
                best_cost = c;
                bv = V_CANDS[i];
                bw = W_CANDS[j];
            }
        }
    }

    status = all_collide ? PLAN_STUCK : PLAN_SEEKING;
    if (all_collide) {
        // Rotate in place toward open space rather than pushing in.
        bv = 0;
        bw = FX_MILLI(0, 700);
    }

    best_v = bv;
    best_w = bw;
    msg_cmd_vel_t cmd = { bv, bw, timer_get_us() };
    topic_publish(t_cmd, &cmd);
}

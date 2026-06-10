// wm.h - the world model: the OS's central belief about the robot and its
// surroundings. Everything downstream (planning, control, telemetry) reads
// the world, and acts on the world, only through this model.
//
// It maintains:
//  - belief state: pose, velocities, with scalar uncertainty that grows
//    during prediction and shrinks when measurements arrive
//  - a forward dynamics model (unicycle) used both to propagate belief
//    between sensor updates and to answer counterfactual queries from the
//    planner ("where would I be after applying (v,w) for t seconds?")
//  - a local obstacle memory built from range returns, kept in world frame
#ifndef WM_H
#define WM_H

#include "fixed.h"
#include "robot_msgs.h"
#include <stdint.h>

#define WM_MAX_OBST_PTS 24

typedef struct {
    fx_t x, y, theta;       // pose estimate (m, rad)
    fx_t v, w;              // velocity estimate (m/s, rad/s)
    fx_t sigma_pos;         // ~1-sigma position uncertainty (m)
    fx_t sigma_theta;       // ~1-sigma heading uncertainty (rad)
    uint64_t stamp_us;
} wm_state_t;

typedef struct {
    fx_t x, y;              // world-frame obstacle return
    uint64_t stamp_us;
} wm_obst_pt_t;

void wm_init(void);
// Periodic update: predict with the dynamics model, then correct from
// whatever sensor messages arrived on the topic bus. Registered as a task.
void wm_update_task(void);
void wm_get(wm_state_t* out);
// Forward-simulate the dynamics model from `from` applying (v, w) for dt.
// This one function is shared by belief prediction, planner rollouts and
// the `predict` shell command - the model is the single source of truth.
void wm_predict(const wm_state_t* from, fx_t v, fx_t w, fx_t dt, wm_state_t* out);
// Obstacle memory accessors.
int wm_obstacle_points(const wm_obst_pt_t** out);
// Smallest distance from (x, y) to any remembered obstacle return.
fx_t wm_clearance(fx_t x, fx_t y);
// Reset pose belief (e.g. on localization reset).
void wm_reset_pose(fx_t x, fx_t y, fx_t theta);

#endif // WM_H

// planner.h - model-predictive planner.
// Plans by imagination: samples candidate (v, w) actions, rolls each one
// forward through the world model's dynamics (wm_predict), scores the
// imagined trajectories against the goal and the obstacle memory, and
// publishes the best action on the cmd_vel topic.
#ifndef PLANNER_H
#define PLANNER_H

#include "fixed.h"
#include <stdint.h>

typedef enum {
    PLAN_IDLE,      // no goal
    PLAN_SEEKING,   // driving toward goal
    PLAN_ARRIVED,
    PLAN_STUCK,     // no candidate trajectory was collision-free
} plan_status_t;

void planner_init(void);
void planner_task(void);
void planner_set_goal(fx_t x, fx_t y);
void planner_stop(void);
plan_status_t planner_status(void);
void planner_goal(fx_t* x, fx_t* y);
// Score/choice of the last planning cycle, for introspection.
fx_t planner_last_best_v(void);
fx_t planner_last_best_w(void);

#endif // PLANNER_H

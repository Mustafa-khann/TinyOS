#include "../include/rt.h"
#include "../include/timer.h"
#include "../include/string.h"

static rt_task_t tasks[RT_MAX_TASKS];
static int task_count = 0;

void rt_init(void) {
    memset(tasks, 0, sizeof(tasks));
    task_count = 0;
}

int rt_register(const char* name, rt_task_fn fn, uint32_t period_us) {
    if (task_count >= RT_MAX_TASKS || !fn) return -1;

    // Insert keeping the array sorted by period (rate-monotonic priority).
    int pos = task_count;
    while (pos > 0 && tasks[pos - 1].period_us > period_us) {
        tasks[pos] = tasks[pos - 1];
        pos--;
    }

    rt_task_t* t = &tasks[pos];
    memset(t, 0, sizeof(*t));
    strncpy(t->name, name, RT_NAME_MAX - 1);
    t->fn = fn;
    t->period_us = period_us;
    t->next_run_us = timer_get_us();
    t->enabled = 1;
    return ++task_count;
}

void rt_set_enabled(int task_id, int enabled) {
    if (task_id > 0 && task_id <= task_count) {
        tasks[task_id - 1].enabled = (uint8_t)enabled;
    }
}

int rt_step(void) {
    int dispatched = 0;
    for (int i = 0; i < task_count; i++) {
        rt_task_t* t = &tasks[i];
        if (!t->enabled) continue;
        uint64_t now = timer_get_us();
        if (now < t->next_run_us) continue;

        if (t->period_us && now > t->next_run_us + t->period_us) {
            t->overruns++;
            // Resynchronize instead of trying to catch up on missed slots.
            t->next_run_us = now;
        }
        t->next_run_us += t->period_us;

        uint64_t start = timer_get_us();
        t->fn();
        uint32_t dur = (uint32_t)(timer_get_us() - start);
        t->last_us = dur;
        if (dur > t->worst_us) t->worst_us = dur;
        t->runs++;
        dispatched++;
    }
    return dispatched;
}

void rt_run(void) {
    for (;;) {
        rt_step();
    }
}

int rt_task_count(void) {
    return task_count;
}

const rt_task_t* rt_task_info(int task_id) {
    if (task_id <= 0 || task_id > task_count) return 0;
    return &tasks[task_id - 1];
}

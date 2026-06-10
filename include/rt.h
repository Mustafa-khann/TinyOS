// rt.h - cooperative real-time task executive.
// Robot control is structured as fixed-rate tasks (sense -> world model ->
// plan -> act). Tasks are run-to-completion; the executive dispatches each
// task when its period elapses, shortest period first (rate-monotonic order).
#ifndef RT_H
#define RT_H

#include <stdint.h>

#define RT_MAX_TASKS 16
#define RT_NAME_MAX  16

typedef void (*rt_task_fn)(void);

typedef struct {
    char name[RT_NAME_MAX];
    rt_task_fn fn;
    uint32_t period_us;     // 0 = run every executive loop
    uint64_t next_run_us;
    uint32_t runs;
    uint32_t overruns;      // dispatched later than one full period
    uint32_t worst_us;      // worst observed execution time
    uint32_t last_us;       // last execution time
    uint8_t  enabled;
} rt_task_t;

void rt_init(void);
int  rt_register(const char* name, rt_task_fn fn, uint32_t period_us);
void rt_set_enabled(int task_id, int enabled);
// Run one scheduling pass (dispatch all due tasks). Returns tasks dispatched.
int  rt_step(void);
// Main loop: rt_step forever.
void rt_run(void) __attribute__((noreturn));
int  rt_task_count(void);
const rt_task_t* rt_task_info(int task_id);

#endif // RT_H

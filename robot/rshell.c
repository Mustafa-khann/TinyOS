#include "../include/rshell.h"
#include "../include/uart.h"
#include "../include/string.h"
#include "../include/fixed.h"
#include "../include/wm.h"
#include "../include/planner.h"
#include "../include/rt.h"
#include "../include/topic.h"
#include "../include/hal.h"
#include "../include/sim.h"
#include "../include/robot.h"
#include "../include/timer.h"
#include "../include/robot_msgs.h"

static void puts_ln(const char* s) {
    uart_puts(s);
    uart_puts("\r\n");
}

static const char* skip_spaces(const char* p) {
    while (*p == ' ') p++;
    return p;
}

// Parse a signed decimal like "-1.25" into Q16.16. Returns end pointer,
// or 0 on parse failure.
static const char* parse_fx(const char* p, fx_t* out) {
    p = skip_spaces(p);
    int neg = 0;
    if (*p == '-') { neg = 1; p++; }
    if (*p < '0' || *p > '9') return 0;

    int32_t ip = 0;
    while (*p >= '0' && *p <= '9') {
        ip = ip * 10 + (*p - '0');
        p++;
    }
    fx_t v = FX_FROM_INT(ip);
    if (*p == '.') {
        p++;
        int32_t frac = 0, div = 1;
        while (*p >= '0' && *p <= '9' && div < 100000) {
            frac = frac * 10 + (*p - '0');
            div *= 10;
            p++;
        }
        v += (fx_t)(((int64_t)frac << FX_SHIFT) / div);
    }
    *out = neg ? -v : v;
    return p;
}

static void print_fx_pair(const char* label, fx_t a, fx_t b) {
    char buf[16];
    uart_puts(label);
    uart_puts(fx_to_str(a, buf));
    uart_puts(", ");
    uart_puts(fx_to_str(b, buf));
    uart_puts("\r\n");
}

static void cmd_wm(void) {
    wm_state_t s;
    wm_get(&s);
    char b[16];
    puts_ln("world model belief:");
    print_fx_pair("  pos   (m):   ", s.x, s.y);
    uart_puts("  theta (rad): "); puts_ln(fx_to_str(s.theta, b));
    print_fx_pair("  vel (m/s, rad/s): ", s.v, s.w);
    print_fx_pair("  sigma (pos, theta): ", s.sigma_pos, s.sigma_theta);
    uart_puts("  clearance here (m): ");
    puts_ln(fx_to_str(wm_clearance(s.x, s.y), b));

    const sim_state_t* t = sim_truth();
    puts_ln("ground truth (sim):");
    print_fx_pair("  pos   (m):   ", t->x, t->y);
    uart_puts("  theta (rad): "); puts_ln(fx_to_str(t->theta, b));
    uart_puts("  collisions: "); puts_ln(itoa((int)t->collisions, 10));

    fx_t ex = s.x - t->x, ey = s.y - t->y;
    uart_puts("  belief error (m): ");
    puts_ln(fx_to_str(fx_sqrt(fx_mul(ex, ex) + fx_mul(ey, ey)), b));
}

static void cmd_world(void) {
    char b[16];
    puts_ln("obstacles (x, y, r):");
    for (int i = 0; i < sim_obstacle_count(); i++) {
        const sim_obstacle_t* o = sim_obstacle(i);
        uart_puts("  ");
        uart_puts(fx_to_str(o->x, b)); uart_puts(", ");
        uart_puts(fx_to_str(o->y, b)); uart_puts(", ");
        puts_ln(fx_to_str(o->r, b));
    }
    const wm_obst_pt_t* pts;
    int n = wm_obstacle_points(&pts);
    int remembered = 0;
    for (int i = 0; i < n; i++) {
        if (pts[i].stamp_us != 0) remembered++;
    }
    uart_puts("range returns remembered: ");
    puts_ln(itoa(remembered, 10));
}

static void cmd_tasks(void) {
    puts_ln("task          period_us  runs      last_us  worst_us  overruns");
    char line[80];
    for (int i = 1; i <= rt_task_count(); i++) {
        const rt_task_t* t = rt_task_info(i);
        line[0] = '\0';
        strcat(line, "  ");
        strcat(line, t->name);
        int pad = 12 - (int)strlen(t->name);
        while (pad-- > 0) strcat(line, " ");
        strcat(line, itoa((int)t->period_us, 10)); strcat(line, "  ");
        strcat(line, itoa((int)t->runs, 10)); strcat(line, "  ");
        strcat(line, itoa((int)t->last_us, 10)); strcat(line, "  ");
        strcat(line, itoa((int)t->worst_us, 10)); strcat(line, "  ");
        strcat(line, itoa((int)t->overruns, 10));
        puts_ln(line);
    }
}

static void cmd_topics(void) {
    puts_ln("topic         msg_size  published");
    char line[64];
    for (int i = 0; i < topic_count(); i++) {
        const topic_t* t = topic_info(i);
        line[0] = '\0';
        strcat(line, "  ");
        strcat(line, t->name);
        int pad = 12 - (int)strlen(t->name);
        while (pad-- > 0) strcat(line, " ");
        strcat(line, itoa(t->msg_size, 10));
        strcat(line, "  ");
        strcat(line, itoa((int)t->seq, 10));
        puts_ln(line);
    }
}

static void cmd_sensors(void) {
    char b[16];
    puts_ln("hal devices:");
    for (int i = 0; i < hal_count(); i++) {
        const hal_device_t* d = hal_info(i);
        uart_puts("  ");
        uart_puts(d->name);
        puts_ln(d->type == HAL_SENSOR ? " (sensor)" : " (actuator)");
    }
    msg_range_t rng;
    topic_t* tr = topic_find(TOPIC_RANGE);
    if (tr && topic_latest(tr, &rng)) {
        uart_puts("range rays (-60..+60 deg, m): ");
        for (int i = 0; i < RANGE_NUM_RAYS; i++) {
            uart_puts(fx_to_str(rng.range[i], b));
            uart_puts(i + 1 < RANGE_NUM_RAYS ? " " : "\r\n");
        }
    }
}

static void cmd_status(void) {
    switch (planner_status()) {
        case PLAN_IDLE:    puts_ln("planner: idle"); break;
        case PLAN_SEEKING: puts_ln("planner: seeking goal"); break;
        case PLAN_ARRIVED: puts_ln("planner: arrived"); break;
        case PLAN_STUCK:   puts_ln("planner: stuck (escaping)"); break;
    }
    if (planner_status() != PLAN_IDLE) {
        fx_t gx, gy;
        planner_goal(&gx, &gy);
        print_fx_pair("  goal (m): ", gx, gy);
        print_fx_pair("  chosen action (v, w): ",
                      planner_last_best_v(), planner_last_best_w());
    }
}

static void cmd_predict(const char* args) {
    fx_t ms;
    if (!parse_fx(args, &ms) || ms <= 0) {
        puts_ln("usage: predict <ms>");
        return;
    }
    wm_state_t s, out;
    wm_get(&s);
    fx_t dt = fx_div(ms, FX_FROM_INT(1000));
    wm_predict(&s, s.v, s.w, dt, &out);
    char b[16];
    uart_puts("in ");
    uart_puts(fx_to_str(ms, b));
    puts_ln(" ms (holding current velocity):");
    print_fx_pair("  predicted pos (m): ", out.x, out.y);
    uart_puts("  predicted theta (rad): ");
    puts_ln(fx_to_str(out.theta, b));
    print_fx_pair("  sigma (pos, theta): ", out.sigma_pos, out.sigma_theta);
}

static void cmd_help(void) {
    puts_ln("robot commands:");
    puts_ln("  wm               - belief state vs ground truth");
    puts_ln("  world            - obstacle map and memory");
    puts_ln("  goto <x> <y>     - drive to (x, y) meters, e.g. goto 2.5 0.4");
    puts_ln("  stop             - cancel goal, stop motors");
    puts_ln("  status           - planner state and chosen action");
    puts_ln("  predict <ms>     - world-model rollout of current motion");
    puts_ln("  sensors          - HAL devices and latest range scan");
    puts_ln("  tasks            - rt executive stats");
    puts_ln("  topics           - message bus stats");
    puts_ln("  telemetry        - toggle 1 Hz state printout");
}

int robot_shell_command(const char* line) {
    line = skip_spaces(line);

    if (strcmp(line, "wm") == 0)        { cmd_wm(); return 1; }
    if (strcmp(line, "world") == 0)     { cmd_world(); return 1; }
    if (strcmp(line, "tasks") == 0)     { cmd_tasks(); return 1; }
    if (strcmp(line, "topics") == 0)    { cmd_topics(); return 1; }
    if (strcmp(line, "sensors") == 0)   { cmd_sensors(); return 1; }
    if (strcmp(line, "status") == 0)    { cmd_status(); return 1; }
    if (strcmp(line, "rhelp") == 0 || strcmp(line, "robot") == 0) {
        cmd_help();
        return 1;
    }
    if (strcmp(line, "telemetry") == 0) {
        robot_telemetry_enable(!robot_telemetry_enabled());
        puts_ln(robot_telemetry_enabled() ? "telemetry on" : "telemetry off");
        return 1;
    }
    if (strcmp(line, "stop") == 0) {
        planner_stop();
        puts_ln("stopped");
        return 1;
    }
    if (strncmp(line, "predict", 7) == 0 && (line[7] == ' ' || line[7] == '\0')) {
        cmd_predict(line + 7);
        return 1;
    }
    if (strncmp(line, "goto", 4) == 0 && (line[4] == ' ' || line[4] == '\0')) {
        fx_t x, y;
        const char* p = parse_fx(line + 4, &x);
        if (!p || !parse_fx(p, &y)) {
            puts_ln("usage: goto <x> <y>   (meters, e.g. goto 2.5 0.4)");
            return 1;
        }
        planner_set_goal(x, y);
        char b[16];
        uart_puts("goal set: ");
        uart_puts(fx_to_str(x, b));
        uart_puts(", ");
        puts_ln(fx_to_str(y, b));
        return 1;
    }
    return 0;
}

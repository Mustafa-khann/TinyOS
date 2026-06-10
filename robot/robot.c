#include "../include/robot.h"
#include "../include/timer.h"
#include "../include/rt.h"
#include "../include/topic.h"
#include "../include/hal.h"
#include "../include/sim.h"
#include "../include/wm.h"
#include "../include/planner.h"
#include "../include/robot_msgs.h"
#include "../include/uart.h"
#include "../include/fixed.h"

void sim_drivers_register(void);

static int telemetry_on = 0;
static topic_t* t_imu;
static topic_t* t_odom;
static topic_t* t_range;
static topic_t* t_cmd;
static uint32_t cur_cmd = 0;

// --- tasks -----------------------------------------------------------

// Ground-truth physics, 200 Hz. Fixed dt keeps integration deterministic.
static void task_sim(void) {
    sim_step(FX_MILLI(0, 5));
}

// Fast proprioception (IMU + encoders), 100 Hz.
static void task_sense_fast(void) {
    hal_device_t* d;
    msg_imu_t imu;
    msg_encoders_t enc;
    if ((d = hal_find("imu0")) && d->read(&imu, sizeof(imu)) > 0) {
        topic_publish(t_imu, &imu);
    }
    if ((d = hal_find("enc0")) && d->read(&enc, sizeof(enc)) > 0) {
        topic_publish(t_odom, &enc);
    }
}

// Exteroception (rangefinder), 20 Hz - raycasts are comparatively costly.
static void task_sense_range(void) {
    hal_device_t* d = hal_find("range0");
    msg_range_t rng;
    if (d && d->read(&rng, sizeof(rng)) > 0) {
        topic_publish(t_range, &rng);
    }
}

// Push the latest velocity command into the motor controller, 50 Hz.
static void task_motor(void) {
    msg_cmd_vel_t cmd;
    hal_device_t* d = hal_find("motor0");
    if (d && topic_poll(t_cmd, &cur_cmd, &cmd)) {
        d->write(&cmd, sizeof(cmd));
    }
}

static void task_telemetry(void) {
    if (!telemetry_on) return;
    wm_state_t s;
    wm_get(&s);
    char b[16];
    uart_puts("[wm] x=");  uart_puts(fx_to_str(s.x, b));
    uart_puts(" y=");      uart_puts(fx_to_str(s.y, b));
    uart_puts(" th=");     uart_puts(fx_to_str(s.theta, b));
    uart_puts(" v=");      uart_puts(fx_to_str(s.v, b));
    uart_puts(" w=");      uart_puts(fx_to_str(s.w, b));
    uart_puts(" sig=");    uart_puts(fx_to_str(s.sigma_pos, b));
    const sim_state_t* t = sim_truth();
    uart_puts(" | truth x="); uart_puts(fx_to_str(t->x, b));
    uart_puts(" y=");         uart_puts(fx_to_str(t->y, b));
    uart_puts("\r\n");
}

// --- bring-up --------------------------------------------------------

void robot_init(void) {
    timer_init();
    topic_init();
    hal_init();
    sim_init();
    sim_drivers_register();
    wm_init();        // advertises imu/odom/range topics
    planner_init();   // advertises cmd_vel topic

    t_imu = topic_find(TOPIC_IMU);
    t_odom = topic_find(TOPIC_ODOM);
    t_range = topic_find(TOPIC_RANGE);
    t_cmd = topic_find(TOPIC_CMD_VEL);

    rt_init();
    rt_register("sim",       task_sim,         5000);    // 200 Hz
    rt_register("sense",     task_sense_fast,  10000);   // 100 Hz
    rt_register("wm",        wm_update_task,   10000);   // 100 Hz
    rt_register("motor",     task_motor,       20000);   // 50 Hz
    rt_register("range",     task_sense_range, 50000);   // 20 Hz
    rt_register("planner",   planner_task,     100000);  // 10 Hz
    rt_register("telemetry", task_telemetry,   1000000); // 1 Hz

    uart_puts(timer_hw_ok()
        ? "[robot] stack up (hw timer)\r\n"
        : "[robot] stack up (WARNING: sw timer fallback)\r\n");
}

void robot_telemetry_enable(int on) {
    telemetry_on = on;
}

int robot_telemetry_enabled(void) {
    return telemetry_on;
}

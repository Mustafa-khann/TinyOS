// robot.h - bring-up and task wiring for the robot stack.
#ifndef ROBOT_H
#define ROBOT_H

// Initializes timer, topic bus, HAL (simulator-backed on QEMU), world
// model, planner and the rt executive, and registers all periodic tasks.
void robot_init(void);
void robot_telemetry_enable(int on);
int robot_telemetry_enabled(void);

#endif // ROBOT_H

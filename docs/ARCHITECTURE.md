# TinyOS Robot Stack Architecture

TinyOS is being grown from a minimal Raspberry Pi hobby kernel into an
operating system for robots, organized around a **world model**: a single,
continuously-updated belief about the robot and its surroundings that every
other part of the system reads from and acts through.

```
                 ┌─────────────────────────────────────────────┐
                 │                  rt executive                │
                 │   (cooperative, rate-monotonic task loop)    │
                 └─────────────────────────────────────────────┘
   sensors (HAL)          topic bus              world model          planner
 ┌──────────────┐      ┌────────────┐      ┌──────────────────┐   ┌───────────┐
 │ imu0         │─────▶│ imu        │─────▶│ predict (dynamics │   │ sample    │
 │ enc0         │─────▶│ odom       │─────▶│  model)           │◀──│ (v,w)     │
 │ range0       │─────▶│ range      │─────▶│ correct (fuse     │   │ rollouts  │
 └──────────────┘      │ cmd_vel    │      │  measurements)    │──▶│ score &   │
 ┌──────────────┐      └────────────┘      │ obstacle memory   │   │ pick best │
 │ motor0       │◀──────── cmd_vel ────────│ uncertainty       │   └───────────┘
 └──────────────┘                          └──────────────────┘         │
        ▲                                                                │
        └────────────────────────── cmd_vel ◀────────────────────────────┘
```

## Why "world models"?

A robot OS is not a desktop OS with motors attached. Its defining job is to
maintain a faithful internal model of the external world and to let software
act through that model:

- **Perception updates the model**, it doesn't drive behavior directly.
  Drivers publish measurements; only the world model decides what to believe.
- **The dynamics model is shared.** The same `wm_predict()` function
  propagates belief between sensor updates, answers the shell's `predict`
  queries, and powers the planner's imagined rollouts. There is exactly one
  notion of "how the world evolves" in the system.
- **Planning is imagination.** The planner never touches sensors or motors.
  It samples candidate actions, rolls each forward through the world model,
  scores the imagined futures, and emits the best action as a message.
- **Uncertainty is first-class.** The belief carries scalar sigmas that grow
  with motion (prediction) and shrink with measurement (correction), so
  downstream code can know how much to trust the model.

## Layers

| Layer | Files | Role |
|---|---|---|
| Boot | `boot/boot.S`, `linker.ld` | Core 0 setup, BSS clear, vector table; links at `0x10000` (QEMU raspi load address) |
| Kernel | `kernel/*.c` | UART, framebuffer/mailbox, shell, allocator, toy FS |
| Time base | `robot/timer.c` | BCM2835 free-running 1 MHz system timer; monotonic `timer_get_us()` |
| Math | `robot/fixed.c` | Q16.16 fixed point: mul/div/sqrt/sin/cos/atan2 (no FPU, no soft-float) |
| Executive | `robot/rt.c` | Cooperative rate-monotonic dispatcher; per-task runtime/overrun stats |
| Messaging | `robot/topic.c` | Pub/sub topics with fixed-size ring buffers, polled cursors |
| HAL | `robot/hal.c`, `robot/sim_drivers.c` | Uniform sensor/actuator interface; QEMU backend is simulator-based |
| Simulator | `robot/sim.c` | Ground-truth differential-drive physics, obstacles, raycasts, noise |
| World model | `robot/wm.c` | Belief state, prediction/correction, obstacle memory, rollout API |
| Planner | `robot/planner.c` | Sampling MPC over world-model rollouts; publishes `cmd_vel` |
| Console | `robot/rshell.c` | UART commands for introspection and goals |

## Task schedule

All robot work runs as run-to-completion tasks under the rt executive
(`rt_run()` replaces the old busy-wait kernel loop):

| Task | Rate | Work |
|---|---|---|
| `sim` | 200 Hz | integrate ground-truth physics (QEMU backend only) |
| `sense` | 100 Hz | read IMU + encoders via HAL, publish `imu`, `odom` |
| `wm` | 100 Hz | predict belief forward, fuse measurements, update obstacle memory |
| `motor` | 50 Hz | forward latest `cmd_vel` to the motor HAL device |
| `range` | 20 Hz | read rangefinder, publish `range` |
| `planner` | 10 Hz | MPC: rollout candidate actions through `wm_predict`, publish best |
| `shell` | 200 Hz | poll UART input |
| `telemetry` | 1 Hz | optional belief-vs-truth printout |

The simulator is deliberately *outside* the belief path: sensors sample it
with noise and bias, and the world model must re-estimate the truth from
those measurements alone — the same code path a real robot would use. The
`wm` shell command prints belief vs. ground truth so estimation error is
always visible.

## The hardware seam

`robot/sim.c` + `robot/sim_drivers.c` are the QEMU backend. On real
hardware, replace `sim_drivers_register()` with drivers that talk I2C/PWM
and register the same `imu0`/`enc0`/`range0`/`motor0` device names. Nothing
above the HAL changes.

## Boot notes (important)

QEMU's raspi machines load raw `-kernel` images at **0x10000**, not the
classic Pi firmware address 0x8000. The kernel is linked at 0x10000 to
match; on real hardware add `kernel_address=0x10000` to `config.txt`.
(The pre-existing 0x8000 link only appeared to work: the BSS clear
overlapped the relocated image and destroyed its own clear loop early,
leaving the OS running by accident.)

CPU IRQs stay masked: the legacy vector table's IRQ slot pointed at its own
literal word, so any stray interrupt executed data. The stack is fully
polled off the hardware timer; a proper IRQ subsystem is future work.

## Roadmap

- Interrupt-driven timing (fix vector table, BCM2836 local timer) and
  preemptive scheduling for hard control deadlines
- Occupancy-grid obstacle memory and scan-matching localization to bound
  drift (currently dead-reckoned with fused velocities)
- Learned/least-squares dynamics calibration: fit the prediction model's
  parameters from logged (action, measurement) pairs instead of constants
- Real sensor drivers (I2C IMU, quadrature encoders, ultrasonic/ToF) behind
  the existing HAL names
- Goal/behavior layer above the MPC (waypoint missions, exploration)

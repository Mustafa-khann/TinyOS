# Tiny-OS

A from-scratch operating system for the Raspberry Pi, developed and tested under QEMU, now growing into a **robot operating system built around world models**: the OS maintains a continuously-updated belief about the robot and its environment, and the entire stack — sensing, prediction, planning, actuation — runs through that model. See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the full design.

**Table of Contents**

* [Introduction](#introduction)
* [Features](#features)
* [Requirements](#requirements)
* [Installation](#installation)
* [Usage](#usage)
* [Development](#development)
* [Project Timeline](#project-timeline)
* [Contributing](#contributing)
* [License](#license)
* [Acknowledgements](#acknowledgements)

**Introduction**

Tiny-Pi-OS is a personal project aimed at creating a simple operating system for the Raspberry Pi. It includes basic functionalities such as a bootloader, a simple kernel, memory management, process scheduling, and basic I/O operations.

**Features**

Robot stack (world-model centric):

* Monotonic time base on the BCM2835 1 MHz system timer
* Cooperative rate-monotonic real-time executive with per-task runtime stats
* Publish/subscribe topic bus (`imu`, `odom`, `range`, `cmd_vel`)
* Sensor/actuator HAL — QEMU backend is a built-in differential-drive simulator with noisy IMU, wheel encoders, 5-ray rangefinder and motor dynamics
* World model: belief state (pose, velocity, uncertainty) with dynamics-model prediction, sensor-fusion correction and obstacle memory
* Model-predictive planner: samples actions, rolls them out through the world model, drives to goals around obstacles
* Q16.16 fixed-point math library (no FPU required)
* Robot console over UART (`wm`, `goto`, `predict`, `tasks`, `topics`, ...)

Base kernel:

* Bootloader in Assembly, kernel in C
* UART and mailbox/framebuffer drivers, text shell
* Basic memory management and in-RAM file system

**Requirements**

* ARM Cross-Compiler (GCC Toolchain)
* QEMU for ARM Emulation
* Make
* Git

**Installation**

### Setting up the Development Environment

1. **Install ARM Cross-Compiler:**
    * On Ubuntu: `sudo apt-get install gcc-arm-none-eabi`
    * On macOS: `brew install arm-none-eabi-gcc`
2. **Install QEMU:**
    * On Ubuntu: `sudo apt-get install qemu-system-arm`
    * On macOS: `brew install qemu`
3. **Clone the Repository:** `git clone https://github.com/mustafa-khann/tinyos.git`

**Usage**

### Building and Running Tiny-Pi-OS

1. **Build the OS:** `make`
2. **Run the OS in QEMU:** `make run` (or `qemu-system-arm -M raspi2b -kernel build/kernel.img -serial stdio`)

Then drive the robot from the serial console:

```
$ rhelp                # list robot commands
$ telemetry            # 1 Hz belief-vs-truth printout
$ goto 2.5 -0.3        # navigate to (x, y) meters, avoiding obstacles
$ status               # planner state and chosen action
$ wm                   # world-model belief vs simulator ground truth
$ predict 500          # roll the world model 500 ms into the future
$ tasks                # real-time executive stats
```

Note: the kernel links at `0x10000` (where QEMU's raspi machines load raw images). On real hardware add `kernel_address=0x10000` to `config.txt`.

**Development**

### Project Structure

* `boot/`: Bootloader (Assembly).
* `build/`: Build output.
* `include/`: Header files.
* `kernel/`: Base kernel (UART, framebuffer, shell, memory, FS).
* `robot/`: Robot stack — timer, executive, topic bus, HAL, simulator, world model, planner, console.
* `docs/`: Architecture documentation.
* `linker.ld`: Linker script.
* `Makefile`: Build instructions.

### Adding New Features

1. **Create a new branch for your feature:** `git checkout -b feature-name`
2. **Make your changes and commit them:** `git add.` and `git commit -m "Description of your feature"`
3. **Push to the branch:** `git push origin feature-name`
4. **Create a pull request on GitHub.**

**Project Timeline**

* Day 1: Environment Setup and Basic Bootloader
* Day 2: Simple Kernel Development
* Day 3: Core Kernel Features
* Day 4: Process Management and Scheduler
* Day 5: Advanced Features
* Day 6: Comprehensive Testing and Debugging
* Day 7: Finalization and Documentation

**Contributing**

Contributions are welcome! Please open an issue or submit a pull request for any improvements or features you would like to see.

**License**

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

**Acknowledgements**

* [Cambridge University Raspberry Pi OS Tutorials](https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/)

---

Let's build something amazing!

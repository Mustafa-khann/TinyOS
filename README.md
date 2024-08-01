# Tiny-Pi-OS

A minimal operating system built from scratch for the Raspberry Pi, developed and tested using QEMU for ARM emulation.

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

* Basic Bootloader in Assembly
* Simple Kernel in C
* Memory Management
* Basic I/O Handling
* Process Management and Scheduling
* File System Support (FAT32)
* Interrupt Handling

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
3. **Clone the Repository:** `git clone https://github.com/mustafa-khann/tiny-pi-os.git`

**Usage**

### Building and Running Tiny-Pi-OS

1. **Build the OS:** `make`
2. **Run the OS in QEMU:** `qemu-system-arm -M versatilepb -kernel kernel.img -serial stdio`

**Development**

### Project Structure

* `boot/`: Contains the bootloader code in Assembly.
* `build/`: Contains build files.
* `include/`: Contains header files.
* `kernel/`: Contains the kernel code in C.
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

# Tiny-Pi-OS

Tiny-Pi-OS is a minimal operating system designed for the Raspberry Pi, developed in C and assembly. This project is an educational endeavor to understand the basics of operating system development, ARM architecture, and low-level programming.

## Project Goals

- Develop a basic bootloader in assembly.
- Implement core kernel features in C.
- Set up basic I/O operations and memory management.
- Implement process management, context switching, and a simple scheduler.
- Add support for basic file systems (e.g., FAT32).
- Optimize and test the operating system using QEMU.

## Project Timeline

### Week 1 (July 29 - August 4)
- Set up development environment.
- Create a basic bootloader.
- Get a "Hello, World!" running on QEMU.

### Week 2 (August 5 - August 11)
- Implement basic kernel features.
- Set up basic I/O and memory management.

### Week 3 (August 12 - August 18)
- Implement process management and task switching.
- Develop a simple scheduler.

### Week 4 (August 19 - August 25)
- Add advanced features like file system support.
- Optimize and debug the OS.

### Final Days (August 26 - August 28)
- Comprehensive testing and finalization.
- Prepare project documentation and report.

## Getting Started

### Prerequisites

- **ARM Cross-Compiler**: Install the GCC toolchain for ARM.
  ```sh
  sudo apt-get install gcc-arm-none-eabi

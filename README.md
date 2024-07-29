# Tiny-Pi-OS

Tiny-Pi-OS is a minimal operating system built from scratch for the Raspberry Pi, developed and tested using QEMU for ARM emulation. This project aims to provide a basic understanding of OS development, including bootloader setup, kernel development, process management, and basic I/O handling.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Development](#development)
- [Project Timeline](#project-timeline)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Introduction

Tiny-Pi-OS is a personal project aimed at creating a simple operating system for the Raspberry Pi. It includes basic functionalities such as a bootloader, a simple kernel, memory management, process scheduling, and basic I/O operations.

## Features

- Basic Bootloader in Assembly
- Simple Kernel in C
- Memory Management
- Basic I/O Handling
- Process Management and Scheduling
- File System Support (FAT32)
- Interrupt Handling

## Requirements

- ARM Cross-Compiler (GCC Toolchain)
- QEMU for ARM Emulation
- Make
- Git

## Installation

### Setting up the Development Environment

1. **Install ARM Cross-Compiler:**

   On Ubuntu:
   ```sh
   sudo apt-get install gcc-arm-none-eabi

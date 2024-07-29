# Compiler and flags
ARMGNU ?= arm-none-eabi
CC = $(ARMGNU)-gcc
AS = $(ARMGNU)-as
LD = $(ARMGNU)-ld
OBJCOPY = $(ARMGNU)-objcopy

CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O2 -Wall -Wextra
ASMFLAGS = -mcpu=cortex-a7
LDFLAGS = -nostdlib

# Directories
SRC_DIR = .
BOOT_DIR = $(SRC_DIR)/boot
BUILD_DIR = $(SRC_DIR)/build
KERNEL_DIR = $(SRC_DIR)/kernel
INCLUDE_DIR = $(SRC_DIR)/include

# Source files
BOOT_SRC = $(BOOT_DIR)/boot.S
KERNEL_SRC = $(KERNEL_DIR)/kernel.c

# Object files
BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o

# Output files
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_IMG = $(BUILD_DIR)/kernel.img

# Default target
all: $(KERNEL_IMG)

# Rule to create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile boot.S
$(BOOT_OBJ): $(BOOT_SRC) | $(BUILD_DIR)
	$(CC) $(ASMFLAGS) -c $< -o $@

# Compile kernel.c
$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link object files
$(KERNEL_ELF): $(BOOT_OBJ) $(KERNEL_OBJ)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $^

# Create binary image
$(KERNEL_IMG): $(KERNEL_ELF)
	$(OBJCOPY) $< -O binary $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean

ARMGNU ?= arm-none-eabi

CC = $(ARMGNU)-gcc
AS = $(ARMGNU)-as
LD = $(ARMGNU)-ld
OBJCOPY = $(ARMGNU)-objcopy

CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -O2 -Wall -Wextra -I./include
ASMFLAGS = -mcpu=cortex-a7

SRC_DIR = .
BUILD_DIR = build

C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
ASM_SOURCES = $(wildcard boot/*.S)
OBJ_FILES = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
OBJ_FILES += $(ASM_SOURCES:%.S=$(BUILD_DIR)/%.o)

# Explicitly add string.o to OBJ_FILES
OBJ_FILES += $(BUILD_DIR)/kernel/string.o

all: $(BUILD_DIR)/kernel.img

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(@D)
	$(AS) $(ASMFLAGS) $< -o $@

# Explicit rule for string.c
$(BUILD_DIR)/kernel/string.o: kernel/string.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(OBJ_FILES)
	$(LD) -T linker.ld -o $@ $^

$(BUILD_DIR)/kernel.img: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) $< -O binary $@

clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)/kernel.img
	qemu-system-arm -M raspi2b -kernel $< -serial stdio

.PHONY: all clean run

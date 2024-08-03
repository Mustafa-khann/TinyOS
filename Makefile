ARMGNU ?= arm-none-eabi
CC = $(ARMGNU)-gcc
AS = $(ARMGNU)-as
LD = $(ARMGNU)-ld
OBJCOPY = $(ARMGNU)-objcopy
OBJDUMP = $(ARMGNU)-objdump

# Remove -O2 for now to disable optimizations
CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding -Wall -Wextra -I./include -g -O0
ASMFLAGS = -mcpu=cortex-a7
LDFLAGS = -T linker.ld -Map=$(BUILD_DIR)/kernel.map

SRC_DIR = .
BUILD_DIR = build

# Explicitly list all C source files
C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
ASM_SOURCES = $(wildcard boot/*.S)
OBJ_FILES = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
OBJ_FILES += $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

all: $(BUILD_DIR)/kernel.img

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(@D)
	$(AS) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/kernel.elf: $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o $@ $^
	$(OBJDUMP) -D $@ > $(BUILD_DIR)/kernel_disassembly.txt
	$(OBJDUMP) -s -j .rodata $@ > $(BUILD_DIR)/rodata_dump.txt

$(BUILD_DIR)/kernel.img: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) $< -O binary $@

clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)/kernel.img
	qemu-system-arm -M raspi2b -kernel $< -serial stdio

debug: $(BUILD_DIR)/kernel.elf
	qemu-system-arm -M raspi2b -kernel $< -serial stdio -s -S

.PHONY: all clean run debug

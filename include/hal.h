// hal.h - hardware abstraction layer for robot peripherals.
// Sensors and actuators register a uniform read/write interface. On QEMU
// the devices are backed by the built-in simulator (robot/sim.c); on real
// hardware the same names would be backed by I2C/PWM drivers, with the rest
// of the stack unchanged.
#ifndef HAL_H
#define HAL_H

#include <stdint.h>

#define HAL_MAX_DEVICES 12
#define HAL_NAME_MAX    16

typedef enum {
    HAL_SENSOR,
    HAL_ACTUATOR,
} hal_type_t;

typedef struct {
    char name[HAL_NAME_MAX];
    hal_type_t type;
    // Sensors fill buf with their message struct; actuators consume it.
    int (*read)(void* buf, int len);
    int (*write)(const void* buf, int len);
} hal_device_t;

void hal_init(void);
int hal_register(const char* name, hal_type_t type,
                 int (*read)(void*, int), int (*write)(const void*, int));
hal_device_t* hal_find(const char* name);
int hal_count(void);
const hal_device_t* hal_info(int index);

#endif // HAL_H

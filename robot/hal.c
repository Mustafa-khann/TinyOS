#include "../include/hal.h"
#include "../include/string.h"

static hal_device_t devices[HAL_MAX_DEVICES];
static int n_devices = 0;

void hal_init(void) {
    memset(devices, 0, sizeof(devices));
    n_devices = 0;
}

int hal_register(const char* name, hal_type_t type,
                 int (*read)(void*, int), int (*write)(const void*, int)) {
    if (n_devices >= HAL_MAX_DEVICES) return -1;
    hal_device_t* d = &devices[n_devices++];
    strncpy(d->name, name, HAL_NAME_MAX - 1);
    d->type = type;
    d->read = read;
    d->write = write;
    return 0;
}

hal_device_t* hal_find(const char* name) {
    for (int i = 0; i < n_devices; i++) {
        if (strcmp(devices[i].name, name) == 0) return &devices[i];
    }
    return 0;
}

int hal_count(void) {
    return n_devices;
}

const hal_device_t* hal_info(int index) {
    if (index < 0 || index >= n_devices) return 0;
    return &devices[index];
}

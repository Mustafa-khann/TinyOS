#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 10
#define PROCESS_NAME_MAX 32

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef struct {
    uint32_t pid;
    char name[PROCESS_NAME_MAX];
    process_state_t state;
    uint32_t* stack_pointer;
    uint32_t stack_base;
    uint32_t stack_size;
} process_t;

void processInit(void);
int createProcess(const char* name, void (*entrypoint)(void));
void terminateProcess(uint32_t pid);
void yieldProcess(void);
process_t* processGetCurrent(void);
void processSchedule(void);

#endif

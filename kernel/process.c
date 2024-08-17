#include "../include/process.h"
#include "../include/memory.h"
#include "../include/string.h"

static process_t processes[MAX_PROCESSES];
static uint32_t next_pid = 1;
static process_t* current_process = NULL;

void process_init(void) {
    memset(processes, 0, sizeof(processes));
}

int createProcess(const char* name, void (*entry_point)(void)) {
    if (next_pid >= MAX_PROCESSES) {
        return -1; // No more process slots available
    }

    process_t* proc = &processes[next_pid];
    proc->pid = next_pid++;
    strncpy(proc->name, name, PROCESS_NAME_MAX - 1);
    proc->name[PROCESS_NAME_MAX - 1] = '\0';
    proc->state = PROCESS_READY;

    // Allocate stack for the process
    proc->stack_size = 4096; // 4KB stack
    proc->stack_base = (uint32_t)malloc(proc->stack_size);
    if (!proc->stack_base) {
        return -1; // Failed to allocate stack
    }

    // Set up initial stack frame
    proc->stack_pointer = (uint32_t*)(proc->stack_base + proc->stack_size);
    *(--proc->stack_pointer) = (uint32_t)entry_point; // PC
    *(--proc->stack_pointer) = 0; // LR
    for (int i = 0; i < 13; i++) {
        *(--proc->stack_pointer) = 0; // R12-R0
    }

    return proc->pid;
}

void terminateProcess(uint32_t pid) {
    if (pid < MAX_PROCESSES && processes[pid].pid == pid) {
        processes[pid].state = PROCESS_TERMINATED;
        free((void*)processes[pid].stack_base);
        if (current_process == &processes[pid]) {
            yieldProcess();
        }
    }
}

void yieldProcess(void) {
    if (current_process) {
        current_process->state = PROCESS_READY;
    }
    processSchedule();
}

process_t* processGetCurrent(void) {
    return current_process;
}

void processSchedule(void) {
    process_t* next = NULL;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_READY) {
            next = &processes[i];
            break;
        }
    }

    if (next) {
        if (current_process) {
            // Save current context
            asm volatile("mov %0, sp" : "=r"(current_process->stack_pointer));
        }

        current_process = next;
        current_process->state = PROCESS_RUNNING;

        // Restore next process context
        asm volatile("mov sp, %0" : : "r"(current_process->stack_pointer));
        asm volatile("pop {r0-r12, lr}");
        asm volatile("pop {pc}");
    }
}

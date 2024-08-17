#include "../include/memory.h"

#define HEAP_SIZE 1048576 // 1 MB heap
#define BLOCK_SIZE 16 // Minimum block size

static uint8_t heap[HEAP_SIZE];
static uint8_t* heap_end = heap + HEAP_SIZE;
static uint8_t* program_break = heap;

void memoryInit(void) {
    program_break = heap;
}

void* malloc(size_t size) {
    if (size == 0) return NULL;

    // Align size to BLOCK_SIZE
    size = (size + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);

    if (program_break + size > heap_end) {
        return NULL; // Out of memory
    }

    void* mem = program_break;
    program_break += size;
    return mem;
}

void free(void* ptr) {
    // This is a very simple free that doesn't actually free memory
    // In a real implementation, you'd want to keep track of free blocks
    // and reuse them in malloc
    (void)ptr; // Suppress unused parameter warning
}

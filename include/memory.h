#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

void memoryInit(void);
void* malloc(size_t size);
void free(void* ptr);

#endif // MEMORY_H

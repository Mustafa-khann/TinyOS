#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void vmmInit(void);
void* vmmAllocPages(size_t numPages);
void vmmFreePages(void* address, size_t numPages);
bool vmmMapPage(uintptr_t virtualAddr, uintptr_t physicalAddr);
void vmmUnmapPage(uintptr_t virtualAddr);

#endif

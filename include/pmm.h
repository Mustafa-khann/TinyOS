#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define PAGESIZE 4096

void pmmInit(uintptr_t memStart, uintptr_t memEnd);
void* pmmAllocPage(void);
void pmmFreePage(void* page);
size_t pmmGetFreePageCount(void);

#endif

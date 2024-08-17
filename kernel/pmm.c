#include "../include/pmm.h"
#include <stdint.h>
#include <string.h>

#define BITMAP_SIZE 32768

static uint32_t bitmap[BITMAP_SIZE];
static uintptr_t memoryStart;
static size_t totalPages;

void pmmInit(uintptr_t memStart, uintptr_t memEnd){
    memoryStart = memStart;
    totalPages = (memEnd - memStart) / PAGESIZE;

    for(size_t i = 0; i<BITMAP_SIZE; i++){
        bitmap[i] = 0;
    }
}

static int findFreePage(void){
    for(size_t i = 0; i<BITMAP_SIZE; i++){
        if(bitmap[i] != 0xFFFFFFFF)
            {
                for(int j = 0; j < 32; j++){
                    if((bitmap[i] & (1 << j)) == 0){
                        return i * 32 + j;
                    }
                }
            }
    }
    return -1;
}

void* pmmAllocPage(void){
    int pageIndex = findFreePage();
    if(pageIndex == -1){
        return NULL;
    }

    bitmap[pageIndex / 32] |= (1 << (pageIndex % 32));
    return (void*) (memoryStart + pageIndex * PAGESIZE);
}

void pmmFreePage(void* page){
    uintptr_t addr = (uintptr_t)page;
    size_t pageIndex = (addr - memoryStart) / PAGESIZE;

    bitmap[pageIndex / 32] &= ~(1 << (pageIndex % 32));
}

size_t pmmGetFreePageCount(void){
    size_t count = 0;
    for(size_t i = 0; i < totalPages; i++){
        if((bitmap[i / 32] & (1 << (i % 32))) == 0){
            count++;
        }
    }
    return count;
}

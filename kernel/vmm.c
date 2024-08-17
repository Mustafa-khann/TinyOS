#include "../include/vmm.h"
#include "../include/pmm.h"
#include <stdint.h>
#include <string.h>

#define PAGE_TABLE_ENTRIES 1024
#define PAGE_DIRECTORY_ENTRIES 1024

typedef uint32_t page_table_entry_t ;
typedef uint32_t page_directory_entry_t ;

static page_directory_entry_t* pageDirectory = NULL;

void vmmInit(void){
    pageDirectory = pmmAllocPage();
    for (int i = 0; i < PAGE_DIRECTORY_ENTRIES; i++){
        pageDirectory[i] = 0;
    }
}

static page_table_entry_t* getPageTable(uintptr_t virtualAddr, bool create){
    uint32_t pdIndex = virtualAddr >> 22;
    if(!(pageDirectory[pdIndex] & 0x1)){
        if(!create){
            return NULL;
        }
        pageDirectory[pdIndex] = (uint32_t)pmmAllocPage() | 0x3;
    }
    return (page_table_entry_t*)(pageDirectory[pdIndex] & 0xFFFFF000);
}

bool vmmMapPage(uintptr_t virtualAddr, uintptr_t physicalAddr){
    page_table_entry_t* pt = getPageTable(virtualAddr, true);
    if(!pt){
        return false;
    }

    uint32_t ptIndex = (virtualAddr >> 12 ) & 0x3FF;
    pt[ptIndex] = physicalAddr | 0x3;
    return true;
}

void vmmUnmapPage(uintptr_t virtualAddr){
    page_table_entry_t* pt = getPageTable(virtualAddr, false);
    if(pt){
        uint32_t ptIndex = (virtualAddr >> 12) & 0x3FF;
        pt[ptIndex] = 0;
    }
}

void* vmmAllocPages(size_t numPages){
    void* startAddr = pmmAllocPage();
    if(!startAddr){
        return NULL;
    }

    uintptr_t virtAddr = (uintptr_t)startAddr;
    for(size_t i = 0; i < numPages; i++){
        void* phys_page = pmmAllocPage();
        if(!phys_page){
            for(size_t j=0; j<i; j++){
                vmmFreePages((void*)(virtAddr + j * PAGESIZE), 1);
            }
            return NULL;
        }
        if(!vmmMapPage(virtAddr + i * PAGESIZE, (uintptr_t)phys_page))
            {
                pmmFreePage(phys_page);
                for(size_t j = 0; j<i; j++){
                    vmmFreePages((void*)(virtAddr + j * PAGESIZE), 1);
                }
                return NULL;
            }
    }
    return (void*)virtAddr;
}

void vmmFreePages(void* address, size_t num_pages) {
    uintptr_t virt_addr = (uintptr_t)address;
    for (size_t i = 0; i < num_pages; i++) {
        page_table_entry_t* pt = getPageTable(virt_addr + i * PAGESIZE, false);
        if (pt) {
            uint32_t pt_index = ((virt_addr + i * PAGESIZE) >> 12) & 0x3FF;
            uintptr_t phys_addr = pt[pt_index] & 0xFFFFF000;
            pmmFreePage((void*)phys_addr);
            vmmUnmapPage(virt_addr + i * PAGESIZE);
        }
    }
}

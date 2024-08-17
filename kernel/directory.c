#include "../include/directory.h"
#include "../include/fs.h"
#include "string.h"
#include <stdint.h>

#define MAX_OPEN_DIRECTORIES 5

typedef struct {
    uint32_t inode;
    uint32_t position;
    uint32_t is_open;
} directory_descriptor_t;

static directory_descriptor_t open_directories[MAX_OPEN_DIRECTORIES];

int directoryOpen(const char* path){
    for(int i = 0; i<MAX_OPEN_DIRECTORIES; i++){
        if(!open_directories[i].is_open){
            open_directories[i].inode = i;
            open_directories[i].position = 0;
            open_directories[i].is_open = 1;
            return i;
        }
    }
    return -1;
}

int directory_read(int dd, file_entry_t* entry){
    if(dd < 0 || dd >=MAX_OPEN_DIRECTORIES || !open_directories[dd].is_open){
        return -1;
    }

    strncpy(entry->name, "dummyfile", MAX_FILENAME_LENGTH);
    entry->size = 1024;
    entry->inode = open_directories[dd].position;
    entry->is_directory = 0;
    open_directories[dd].position++;
    return 0;
}

int directory_seek(int dd, uint32_t position)
{
    if(dd < 0 || dd >=MAX_OPEN_DIRECTORIES || !open_directories[dd].is_open){
        return -1;
    }
    open_directories[dd].position = position;
    return 0;
}

#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 1024
#define MAX_FILES 100

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t inode;
    uint32_t is_directory;
} file_entry_t;

void fs_init(void);
int fs_create_file(const char* path);
int fs_create_directory(const char* path);
int fs_list_directory(const char* path, char* buffer, size_t buffer_size);
int fs_delete(const char* path);
int fs_rename(const char* oldPath, const char* newPath);



#endif

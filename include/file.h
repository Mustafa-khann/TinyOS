#ifndef FILE_H
#define FILE_H

#include <stdint.h>

typedef struct {
    uint32_t inode;
    uint32_t position;
    uint8_t is_open;
} file_descriptor_t;

void file_open(const char* path);
int file_close(int fd);
int file_read(int fd, void* buffer, uint32_t count);
int file_write(int fd, const void* buffer, uint32_t count);
int file_seek(int fd, int32_t offset, int whence);

#endif

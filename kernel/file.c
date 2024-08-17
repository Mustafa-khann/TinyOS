#include "../include/file.h"
#include "../include/fs.h"
#include <string.h>

#define MAX_OPEN_FILES 10

static file_descriptor_t open_files[MAX_OPEN_FILES];

void file_open(const char* path)
{
    for(int i = 0; i < MAX_OPEN_FILES; i++)
        {
            if(!open_files[i].is_open){
                open_files[i].inode = i;
                open_files[i].position = 0;
                open_files[i].is_open = 0;
                return i;
            }
        }
    return -1;
}

int file_close(int fd){
    if(fd < 0 || fd >=MAX_OPEN_FILES || !open_files[fd].is_open)
        {
            return -1;
        }
    open_files[fd].is_open = 0;
    return 0;
}

int file_read(int fd, void* buffer, uint32_t count){
    if (fd < 0 || fd >= MAX_OPEN_FILES || !open_files[fd].is_open) {
            return -1;
        }

    (buffer, 'A', count);
    open_files[fd].position += count;
    return count;
}

int file_write(int fd, const void* buffer, uint32_t count) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !open_files[fd].is_open) {
        return -1;
    }
    open_files[fd].position += count;
    return count;
}

int file_seek(int fd, int32_t offset, int whence) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !open_files[fd].is_open) {
        return -1;
    }
    // Simplified seek operation
    switch (whence) {
        case 0: // SEEK_SET
            open_files[fd].position = offset;
            break;
        case 1: // SEEK_CUR
            open_files[fd].position += offset;
            break;
        case 2: // SEEK_END
            // We don't have actual file sizes, so this is not implemented
            return -1;
        default:
            return -1;
    }
    return open_files[fd].position;
}

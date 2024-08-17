#include "../include/fs.h"
#include "../include/string.h"

#include <stdint.h>
#include <string.h>

static file_entry_t root_directory[MAX_FILES];
static uint32_t num_files = 0;

void fs_init(void){
    memset(root_directory, 0, sizeof(root_directory));
    num_files = 0;
}

int fs_create_file(const char *path){
    if(num_files >=MAX_FILES){
        return -1;
    }

    file_entry_t* new_file = &root_directory[num_files];
    strncpy(new_file->name, path, MAX_FILENAME_LENGTH - 1);
    new_file -> size = 0;
    new_file -> inode = num_files;
    new_file -> is_directory = 0;

    num_files++;
    return 0;
}

int fs_create_directory(const char* path){
    if(num_files >=MAX_FILES){
        return -1;
    }

    file_entry_t* new_dir = &root_directory[num_files];
    strncpy(new_dir->name, path, MAX_FILENAME_LENGTH - 1);
    new_dir -> size = 0;
    new_dir -> inode = num_files;
    new_dir -> is_directory = 1;

    num_files++;
    return 0;
}

int fs_list_directory(const char* path, char* buffer, size_t buffer_size) {
    // For simplicity, we'll ignore the path for now and just list the root directory
    size_t offset = 0;
    for (uint32_t i = 0; i < num_files; i++) {
        size_t entry_len = strlen(root_directory[i].name) + 2; // +2 for newline and potential '/'
        if (offset + entry_len > buffer_size - 1) // -1 for null terminator
            break;

        strcpy(buffer + offset, root_directory[i].name);
        offset += strlen(root_directory[i].name);

        if (root_directory[i].is_directory) {
            buffer[offset++] = '/';
        }
        buffer[offset++] = '\n';
    }
    buffer[offset] = '\0';
    return 0;
}

int fs_delete(const char* path){
    for(uint32_t i = 0; i < num_files; i++){
        if(strcmp(root_directory[i].name, path) == 0){
            memmove(&root_directory[i], &root_directory[i+1], (num_files - i - 1) * sizeof(file_entry_t));
            num_files--;
            return 0;
        }
    }
    return -1;
}

int fs_rename(const char* old_path, const char* new_path) {
    for(uint32_t i = 0; i < num_files; i++){
        if(strcmp (root_directory[i].name, old_path) == 0){
            strncpy(root_directory[i].name, new_path, MAX_FILENAME_LENGTH - 1);
            return 0;
        }
    }
    return -1;
}

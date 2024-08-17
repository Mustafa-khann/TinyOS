#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "fs.h"

int directory_open(const char* path);
int directory_close(int dd);
int directory_read(int dd, file_entry_t* entry);
int directory_seek(int dd, uint32_t position);

#endif

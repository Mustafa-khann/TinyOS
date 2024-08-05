// include/string.h
#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void* memset(void* s, int c, size_t n);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);

#endif // STRING_H

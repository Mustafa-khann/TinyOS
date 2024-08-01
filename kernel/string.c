// kernel/string.c

#include <stddef.h>

void* __attribute__((used)) memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

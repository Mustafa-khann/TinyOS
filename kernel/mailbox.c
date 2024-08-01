#include "../include/mailbox.h"

#define MAILBOX_BASE    0x3F00B880
#define MAILBOX_READ    ((volatile unsigned int*)(MAILBOX_BASE + 0x00))
#define MAILBOX_STATUS  ((volatile unsigned int*)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE   ((volatile unsigned int*)(MAILBOX_BASE + 0x20))

void mailbox_write(unsigned char channel, unsigned int data) {
    while (*MAILBOX_STATUS & 0x80000000);
    *MAILBOX_WRITE = ((data & 0xFFFFFFF0) | (channel & 0xF));
}

unsigned int mailbox_read(unsigned char channel) {
    unsigned int data;
    do {
        while (*MAILBOX_STATUS & 0x40000000);
        data = *MAILBOX_READ;
    } while ((data & 0xF) != channel);
    return data & 0xFFFFFFF0;
}

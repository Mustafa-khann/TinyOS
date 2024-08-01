#ifndef MAILBOX_H
#define MAILBOX_H

void mailbox_write(unsigned char channel, unsigned int data);
unsigned int mailbox_read(unsigned char channel);

#endif

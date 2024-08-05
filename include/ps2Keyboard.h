#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

void ps2Keyboard_init(void);
void ps2Keyboard_handler(void);
int ps2Keyboard_getc(void);
char scancode_to_ascii(unsigned char scancode);

#endif // PS2_KEYBOARD_H

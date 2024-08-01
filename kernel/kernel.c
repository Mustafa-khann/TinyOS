#include "../include/frameBuffer.h"

void kernel_main(void) {
    framebuffer_init();

    // Draw a white rectangle
    for (int y = 300; y < 400; y++) {
        for (int x = 300; x < 500; x++) {
            put_pixel(x, y, 255, 150, 255);
        }
    }

    draw_string("Welcome to TinyOS!", 110, 150, 255,255,255);  // Black text on white background

    while(1) {}
}

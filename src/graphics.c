#include <stdio.h>
#include <string.h>
#include "../include/graphics.h"

void fb_clear(Framebuffer *fb) {
    fb->width  = SCREEN_WIDTH;
    fb->height = SCREEN_HEIGHT;
    for (int y = 0; y < fb->height; y++)
        for (int x = 0; x < fb->width; x++) {
            fb->cells[y][x].ch     = ' ';
            fb->cells[y][x].fg     = 37;
            fb->cells[y][x].depth  = 1e9f;
            fb->cells[y][x].obj_id = -1;
        }
}

void fb_present(const Framebuffer *fb) {
    fputs("\033[H", stdout);

    for (int y = 0; y < fb->height; y++) {
        for (int x = 0; x < fb->width; x++) {
            const FrameCell *c = &fb->cells[y][x];
            /* Simple foreground colour via ANSI */
            printf("\033[%dm%c", c->fg, c->ch);
        }
        /* Reset colour, newline */
        fputs("\033[0m\n", stdout);
    }
    fflush(stdout);
}

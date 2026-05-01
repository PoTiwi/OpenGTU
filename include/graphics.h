#ifndef OPENGTU_GRAPHICS_H
#define OPENGTU_GRAPHICS_H

#include <stdint.h>
#include "config.h"

typedef struct {
    char shade;
    char physical;
    char p_closer;
    char p_further;
    char wire;
    char highlight;
    char shadow;
    char empty;
} __structGS;

static inline __structGS __retu_gst(void){
    return (__structGS){ '/', '#', '%', '*', '+', '@', '.', ' ' };
}

static inline __structGS __retu_gst_dense(void){
    return (__structGS){ 'H', 'M', 'W', 'x', '+', 'O', ':', ' ' };
}

static inline __structGS __retu_gst_minimal(void){
    return (__structGS){ '.', 'o', 'O', ',', '-', '*', ' ', ' ' };
}

static inline __structGS __retu_gst_classic(void){
    return (__structGS){ '+', '=', '#', '-', '|', '*', '~', ' ' };
}

static inline __structGS __retu_gst_smooth(void){
    return (__structGS){ ';', 'c', 'o', '!', '|', '$', '`', ' ' };
}

static inline char depth_shade(const __structGS *gs,
                                float depth, float light)
{
    float v = (1.0f - depth) * 0.6f + light * 0.4f;
    if (v > 0.85f) return gs->highlight;
    if (v > 0.65f) return gs->p_closer;
    if (v > 0.45f) return gs->physical;
    if (v > 0.30f) return gs->shade;
    if (v > 0.15f) return gs->p_further;
    return gs->shadow;
}

/* --- Framebuffer cell -------------------------------------- */
typedef struct {
    char    ch;
    uint8_t fg;
    float   depth;
    int     obj_id;   /* -1 = background                    */
} FrameCell;

/* --- Framebuffer ------------------------------------------- */
typedef struct {
    FrameCell cells[SCREEN_HEIGHT][SCREEN_WIDTH];
    int width, height;
} Framebuffer;

void fb_clear  (Framebuffer *fb);
void fb_present(const Framebuffer *fb);

static inline void fb_put(Framebuffer *fb, int x, int y,
                           char ch, float depth, int obj_id)
{
    if (x < 0 || x >= fb->width || y < 0 || y >= fb->height) return;
    FrameCell *c = &fb->cells[y][x];
    if (depth < c->depth){ c->ch=ch; c->depth=depth; c->obj_id=obj_id; }
}

#endif /* OPENGTU_GRAPHICS_H */

#ifndef OPENGTU_RENDERER_H
#define OPENGTU_RENDERER_H

#include "graphics.h"
#include "camera.h"
#include "object.h"

typedef struct {
    Framebuffer fb_front;
    Framebuffer fb_back;
    bool        wireframe;
    bool        debug;
    int         shade_profile;
} Renderer;

void renderer_init       (Renderer *r);
void renderer_begin_frame(Renderer *r);
void renderer_draw_scene (Renderer *r, const Scene *s,
                           const Camera *cam);
void renderer_draw_debug (Renderer *r, const Scene *s,
                           const Camera *cam, double fps);
void renderer_end_frame  (Renderer *r);

void raster_triangle(Framebuffer *fb,
    int x0,int y0,float z0,float l0,
    int x1,int y1,float z1,float l1,
    int x2,int y2,float z2,float l2,
    __structGS *gs,
    int obj_id);

void raster_line    (Framebuffer *fb,
                     int x0,int y0, int x1,int y1,
                     char ch, int obj_id);

#endif /* OPENGTU_RENDERER_H */

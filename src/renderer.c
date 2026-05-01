#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/renderer.h"
#include <stdlib.h>

static inline float min3(float a,float b,float c){ return fminf(a,fminf(b,c)); }
static inline float max3(float a,float b,float c){ return fmaxf(a,fmaxf(b,c)); }

static inline float edge(int x0,int y0,int x1,int y1,int x,int y){
    return (float)(x - x0)*(y1 - y0) - (float)(y - y0)*(x1 - x0);
}

void renderer_init(Renderer *r) {
    memset(r, 0, sizeof(*r));
    r->fb_front.width  = r->fb_back.width  = SCREEN_WIDTH;
    r->fb_front.height = r->fb_back.height = SCREEN_HEIGHT;
    r->debug    = DEBUG_OVERLAY;
    r->wireframe= WIREFRAME_MODE;
    r->shade_profile = 0;

    fputs("\033[?25l\033[2J", stdout);
}

void renderer_begin_frame(Renderer *r) {
    fb_clear(&r->fb_back);
}

void raster_line(Framebuffer *fb,
                 int x0,int y0, int x1,int y1,
                 char ch, int obj_id)
{
    int dx=abs(x1-x0), sx=x0<x1?1:-1;
    int dy=-abs(y1-y0),sy=y0<y1?1:-1;
    int err=dx+dy, e2;

    while(1){
        fb_put(fb, x0,y0, ch, 0.0f, obj_id);
        if(x0==x1&&y0==y1) break;
        e2=2*err;
        if(e2>=dy){err+=dy;x0+=sx;}
        if(e2<=dx){err+=dx;y0+=sy;}
    }
}

void raster_triangle(Framebuffer *fb,
    int x0,int y0,float z0,float l0,
    int x1,int y1,float z1,float l1,
    int x2,int y2,float z2,float l2,
    __structGS *gs,
    int obj_id)
{
    int minx = (int)fmaxf(0, floorf(min3(x0,x1,x2)));
    int maxx = (int)fminf(fb->width-1,  ceilf(max3(x0,x1,x2)));
    int miny = (int)fmaxf(0, floorf(min3(y0,y1,y2)));
    int maxy = (int)fminf(fb->height-1, ceilf(max3(y0,y1,y2)));

    float iz0 = 1.0f / z0;
    float iz1 = 1.0f / z1;
    float iz2 = 1.0f / z2;

    float lz0 = l0 * iz0;
    float lz1 = l1 * iz1;
    float lz2 = l2 * iz2;

    float area = edge(x0,y0,x1,y1,x2,y2);
    if (area == 0) return;

    for(int y=miny; y<=maxy; y++){
        for(int x=minx; x<=maxx; x++){

            float w0 = edge(x1,y1,x2,y2,x,y);
            float w1 = edge(x2,y2,x0,y0,x,y);
            float w2 = edge(x0,y0,x1,y1,x,y);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0){

                w0 /= area;
                w1 /= area;
                w2 /= area;

                float inv_z = w0*iz0 + w1*iz1 + w2*iz2;
                float depth = 1.0f / inv_z;

                float light = (w0*lz0 + w1*lz1 + w2*lz2) / inv_z;

                char ch = depth_shade(gs, depth, light);

                fb_put(fb, x,y, ch, depth, obj_id);
            }
        }
    }
}

void renderer_draw_scene(Renderer *r, const Scene *s,
                          const Camera *cam)
{
    Framebuffer *fb = &r->fb_back;

    __structGS gs;
    switch(r->shade_profile % 5){
        case 1:  gs=__retu_gst_dense();   break;
        case 2:  gs=__retu_gst_minimal(); break;
        case 3:  gs=__retu_gst_classic(); break;
        case 4:  gs=__retu_gst_smooth();  break;
        default: gs=__retu_gst();         break;
    }

    for (int oi=0; oi<s->obj_count; oi++) {
        const Object *obj = &s->objects[oi];
        if (!obj->active || obj->transparency >= 1.0f) continue;

        int wf = r->wireframe || obj->wireframe;

        for (int fi=0; fi<obj->mesh.face_count; fi++) {

            int ia=obj->mesh.faces[fi].a;
            int ib=obj->mesh.faces[fi].b;
            int ic=obj->mesh.faces[fi].c;

            Vec3 wa=mat4_transform_point(obj->world_matrix, obj->mesh.verts[ia]);
            Vec3 wb=mat4_transform_point(obj->world_matrix, obj->mesh.verts[ib]);
            Vec3 wc=mat4_transform_point(obj->world_matrix, obj->mesh.verts[ic]);

            Vec3 wn=mat4_transform_dir(obj->world_matrix, obj->mesh.normals[fi]);
            Vec3 to_cam=v3_sub(cam->position, wa);
            if(v3_dot(wn, to_cam) < 0) continue;

            int ax,ay,bx,by,cx,cy;
            float az,bz,cz;

            if(!camera_project(cam,wa,&ax,&ay,&az)) continue;
            if(!camera_project(cam,wb,&bx,&by,&bz)) continue;
            if(!camera_project(cam,wc,&cx,&cy,&cz)) continue;

            if (az <= 0 || bz <= 0 || cz <= 0) continue;

            float light = object_light_at_face((Object*)obj, fi, (Scene*)s);
            float la = light;
            float lb = light;
            float lc = light;
            
            if (wf) {
                char ch = gs.wire;
                raster_line(fb,ax,ay,bx,by,ch,obj->id);
                raster_line(fb,bx,by,cx,cy,ch,obj->id);
                raster_line(fb,cx,cy,ax,ay,ch,obj->id);
            } else {
                raster_triangle(fb,
                    ax,ay,az,la,
                    bx,by,bz,lb,
                    cx,cy,cz,lc,
                    &gs,
                    obj->id);
            }
        }
    }
}

void renderer_draw_debug(Renderer *r, const Scene *s,
                          const Camera *cam, double fps)
{
    if (!r->debug) return;

    char buf[SCREEN_WIDTH];
    snprintf(buf, sizeof(buf),
        " OpenGTU | FPS:%.1f | Objs:%d | Cam:(%.1f,%.1f,%.1f) ",
        fps, s->obj_count,
        cam->position.x, cam->position.y, cam->position.z);

    Framebuffer *fb = &r->fb_back;

    for (int x=0; buf[x] && x<fb->width; x++) {
        fb->cells[0][x].ch    = buf[x];
        fb->cells[0][x].depth = -1.0f;
    }
}

void renderer_end_frame(Renderer *r) {
    fputs("\033[H", stdout);

    r->fb_front = r->fb_back;
    fb_present(&r->fb_front);
}
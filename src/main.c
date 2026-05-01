#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "../include/config.h"
#include "../include/math3d.h"
#include "../include/graphics.h"
#include "../include/object.h"
#include "../include/camera.h"
#include "../include/renderer.h"
#include "../include/physics.h"
#include "../include/input.h"
#include "../include/animation.h"
#include "../include/scene_io.h"

/* ============================================================
 *  OpenGTU — Main engine loop
 * ============================================================ */

/* Portable high-res time in seconds */
static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void build_demo_scene(Scene *s, AnimationSystem *anim) {
    /* Ambient + directional light */
    s->ambient = (Vec3){0.15f, 0.15f, 0.15f};
    s->gravity = (Vec3){0, -GRAVITY, 0};

    s->lights[0].directional = true;
    s->lights[0].direction   = v3_norm((Vec3){1, -1, 0.5f});
    s->lights[0].intensity   = 0.9f;
    s->light_count = 1;

    /* Ground plane */
    Object *plane = scene_add_object(s, SHAPE_PLANE,
        (Vec3){0,-1,0}, v3_zero(), (Vec3){5,1,5});
    plane->anchored  = true;
    plane->shading   = __retu_gst_classic();

    /* Spinning cube */
    Object *cube = scene_add_object(s, SHAPE_CUBE,
        (Vec3){-2, 0, 0}, v3_zero(), v3_one());
    int spin = anim_create(anim, cube->id, true);
    anim_add_key(anim, spin, (Keyframe){0.0f, {-2,0,0},{0,0,0},{1,1,1}});
    anim_add_key(anim, spin, (Keyframe){2.0f, {-2,0,0},{0,(float)M_PI,0},{1,1,1}});
    anim_add_key(anim, spin, (Keyframe){4.0f, {-2,0,0},{0,2*(float)M_PI,0},{1,1,1}});
    anim_play(anim, spin);

    /* Sphere */
    scene_add_object(s, SHAPE_SPHERE,
        (Vec3){1, 0, 1}, v3_zero(), (Vec3){0.8f,0.8f,0.8f});

    /* Cone */
    scene_add_object(s, SHAPE_CONE,
        (Vec3){3, 0, -1}, v3_zero(), v3_one());

    /* Cylinder */
    scene_add_object(s, SHAPE_CYLINDER,
        (Vec3){-3, 0, 2}, v3_zero(), (Vec3){0.6f,1.2f,0.6f});

    /* Custom voxel shape (square with gap) */
    Object *custom = scene_add_object(s, SHAPE_CUSTOM,
        (Vec3){0, 1, 3}, v3_zero(), (Vec3){0.3f,0.3f,0.3f});
    custom->vox_w = SHAPE_SQUARE_W;
    custom->vox_h = SHAPE_SQUARE_H;
    custom->vox_d = SHAPE_SQUARE_D;
    memcpy(custom->voxels, shape_square,
           SHAPE_SQUARE_W * SHAPE_SQUARE_H * SHAPE_SQUARE_D * sizeof(int));
    object_build_mesh(custom);

    /* Bouncing sphere (physics demo) */
    Object *ball = scene_add_object(s, SHAPE_SPHERE,
        (Vec3){0, 5, 0}, v3_zero(), (Vec3){0.5f,0.5f,0.5f});
    ball->body.velocity = (Vec3){0.8f, 0, 0};
}

int main(int argc, char *argv[]) {
    /* ---- Startup ----------------------------------------- */
    printf("OpenGTU v1.0 — ASCII 3-D Terminal Engine\n");
    printf("Controls: WASD=move  Arrows=look  Q/E=up/dn\n");
    printf("          F1=wireframe  F2=debug  F3=shade  ESC=quit\n");
    sleep(1);

    input_init();

    Scene *scene = calloc(1, sizeof(Scene));
    Camera *cam  = malloc(sizeof(Camera));
    Renderer *ren = malloc(sizeof(Renderer));
    AnimationSystem *anim = malloc(sizeof(AnimationSystem));

    if (!scene || !cam || !ren || !anim) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    anim_init(anim);
    renderer_init(ren);

    camera_init(cam, (camera == 1) ? CAM_FIXED : CAM_FREE);

    if (argc >= 2) {
        if (scene_load(scene, argv[1]) != 0) {
            fprintf(stderr, "Warning: could not load '%s', using demo scene.\n",
                    argv[1]);
            build_demo_scene(scene, anim);
        }
    } else {
        build_demo_scene(scene, anim);
    }

    const double frame_time = 1.0 / FPS;
    const double tick_time  = 1.0 / __GTR;
    double last_render = now_seconds();
    double last_tick   = now_seconds();
    double fps_timer   = 0;
    int    fps_frames  = 0;
    double fps_display = 0;

    while (1) {
        double t_now = now_seconds();

        int key = input_poll();
        float dt = (float)frame_time;

        if (key == KEY_ESC) break;
        if (key == KEY_F1) ren->wireframe     = !ren->wireframe;
        if (key == 'f') ren->debug         = !ren->debug;
        if (key == 'g') ren->shade_profile = (ren->shade_profile + 1) % 5;

        if (cam->mode == CAM_FREE) {
            if (key=='w'||input_held('w')) camera_move_forward (cam, dt);
            if (key=='s'||input_held('s')) camera_move_backward(cam, dt);
            if (key=='a'||input_held('a')) camera_move_left    (cam, dt);
            if (key=='d'||input_held('d')) camera_move_right   (cam, dt);
            if (key=='q'||input_held('q')) camera_move_up      (cam, dt);
            if (key=='e'||input_held('e')) camera_move_down    (cam, dt);
            if (key==KEY_LEFT ) camera_look_left (cam, dt);
            if (key==KEY_RIGHT) camera_look_right(cam, dt);
            if (key==KEY_UP   ) camera_look_up   (cam, dt);
            if (key==KEY_DOWN ) camera_look_down (cam, dt);
        } else {
            if (key==KEY_LEFT ) camera_orbit_yaw  (cam, -0.05f);
            if (key==KEY_RIGHT) camera_orbit_yaw  (cam,  0.05f);
            if (key==KEY_UP   ) camera_orbit_pitch(cam,  0.05f);
            if (key==KEY_DOWN ) camera_orbit_pitch(cam, -0.05f);
            if (key=='w')       camera_orbit_zoom (cam, -0.2f);
            if (key=='s')       camera_orbit_zoom (cam,  0.2f);
        }

        if (t_now - last_tick >= tick_time) {
            float tick_dt = (float)(t_now - last_tick);
            anim_update(anim, scene, tick_dt);
            physics_update(scene, tick_dt);
            last_tick = t_now;
        }

        if (t_now - last_render >= frame_time) {
            double frame_dt = t_now - last_render;
            last_render = t_now;

            fps_frames++;
            fps_timer += frame_dt;
            if (fps_timer >= 0.5) {
                fps_display = fps_frames / fps_timer;
                fps_frames  = 0;
                fps_timer   = 0;
            }

            renderer_begin_frame(ren);
            renderer_draw_scene (ren, scene, cam);
            renderer_draw_debug (ren, scene, cam, fps_display);
            renderer_end_frame  (ren);
        }

        double sleep_s = frame_time - (now_seconds() - t_now);
        if (sleep_s > 0) {
            struct timespec ts = {
                .tv_sec  = (time_t)sleep_s,
                .tv_nsec = (long)((sleep_s - (time_t)sleep_s) * 1e9)
            };
            nanosleep(&ts, NULL);
        }
    }

    input_cleanup();

    free(scene);
    free(cam);
    free(ren);
    free(anim);

    printf("\nOpenGTU shut down. Bye!\n");
    return 0;
}
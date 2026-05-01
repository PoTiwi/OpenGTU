#ifndef OPENGTU_CAMERA_H
#define OPENGTU_CAMERA_H

#include <stdbool.h>
#include "math3d.h"
#include "config.h"

typedef enum { CAM_FIXED = 0, CAM_FREE = 1 } CameraMode;

typedef struct {
    CameraMode mode;

    Vec3  position;
    Vec3  target;
    Vec3  up;

    float yaw, pitch;

    float fov, near_clip, far_clip, aspect;

    float orbit_radius, orbit_yaw, orbit_pitch;

    Mat4 view, proj, vp;
} Camera;

void camera_init   (Camera *cam, CameraMode mode);
void camera_update (Camera *cam);

void camera_move_forward (Camera *cam, float dt);
void camera_move_backward(Camera *cam, float dt);
void camera_move_left    (Camera *cam, float dt);
void camera_move_right   (Camera *cam, float dt);
void camera_move_up      (Camera *cam, float dt);
void camera_move_down    (Camera *cam, float dt);

void camera_look_left    (Camera *cam, float dt);
void camera_look_right   (Camera *cam, float dt);
void camera_look_up      (Camera *cam, float dt);
void camera_look_down    (Camera *cam, float dt);

void camera_orbit_yaw    (Camera *cam, float delta);
void camera_orbit_pitch  (Camera *cam, float delta);
void camera_orbit_zoom   (Camera *cam, float delta);

bool camera_project(const Camera *cam, Vec3 world_pt,
                    int *sx, int *sy, float *depth);

#endif /* OPENGTU_CAMERA_H */

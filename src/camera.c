#include <math.h>
#include "../include/camera.h"

#define PITCH_LIMIT 1.5f

static inline float clampf(float v, float min, float max) {
    return v < min ? min : (v > max ? max : v);
}


static inline Vec3 cam_forward(const Camera *cam) {
    return v3_norm(v3_sub(cam->target, cam->position));
}

static Vec3 cam_right(const Camera *cam) {
    Vec3 forward = cam_forward(cam);
    return v3_norm(v3_cross(forward, cam->up));
}

void camera_init(Camera *cam, CameraMode mode) {
    cam->mode         = mode;
    cam->position     = (Vec3){0, 2, -6};
    cam->target       = (Vec3){0, 0,  0};
    cam->up           = (Vec3){0, 1,  0};

    cam->yaw          = 0.0f;
    cam->pitch        = 0.0f;

    cam->fov          = FOV;
    cam->near_clip    = 0.1f;
    cam->far_clip     = RENDER_DIST;

    cam->aspect = (float)SCREEN_WIDTH / ((float)SCREEN_HEIGHT * 2.0f);

    cam->orbit_radius = 6.0f;
    cam->orbit_yaw    = 0.3f;
    cam->orbit_pitch  = 0.4f;

    camera_update(cam);
}

void camera_update(Camera *cam) {
    if (cam->mode == CAM_FIXED) {

        float cy = cosf(cam->orbit_yaw);
        float sy = sinf(cam->orbit_yaw);
        float cp = cosf(cam->orbit_pitch);
        float sp = sinf(cam->orbit_pitch);

        cam->position = (Vec3){
            cam->orbit_radius * cy * cp,
            cam->orbit_radius * sp,
            cam->orbit_radius * sy * cp
        };
        

        cam->target = (Vec3){0, 0, 0};
    } else {

        cam->pitch = clampf(cam->pitch, -PITCH_LIMIT, PITCH_LIMIT);

        Vec3 forward = {
            cosf(cam->pitch) * sinf(cam->yaw),
            sinf(cam->pitch),
            cosf(cam->pitch) * cosf(cam->yaw)
        };

        cam->target = v3_add(cam->position, forward);
    }

    cam->view = mat4_look_at(cam->position, cam->target, cam->up);
    cam->proj = mat4_perspective(cam->fov, cam->aspect,
                                 cam->near_clip, cam->far_clip);
    cam->vp   = mat4_mul(cam->proj, cam->view);
}


void camera_move_forward(Camera *c, float dt) {
    c->position = v3_add(c->position,
        v3_scale(cam_forward(c), INPUT_MOVE_SPEED * dt));
    camera_update(c);
}

void camera_move_backward(Camera *c, float dt) {
    c->position = v3_sub(c->position,
        v3_scale(cam_forward(c), INPUT_MOVE_SPEED * dt));
    camera_update(c);
}

void camera_move_left(Camera *c, float dt) {
    c->position = v3_sub(c->position,
        v3_scale(cam_right(c), INPUT_MOVE_SPEED * dt));
    camera_update(c);
}

void camera_move_right(Camera *c, float dt) {
    c->position = v3_add(c->position,
        v3_scale(cam_right(c), INPUT_MOVE_SPEED * dt));
    camera_update(c);
}

void camera_move_up(Camera *c, float dt) {
    c->position.y += INPUT_MOVE_SPEED * dt;
    camera_update(c);
}

void camera_move_down(Camera *c, float dt) {
    c->position.y -= INPUT_MOVE_SPEED * dt;
    camera_update(c);
}

void camera_look(Camera *c, float dx, float dy, float dt) {
    c->yaw   += dx * INPUT_LOOK_SPEED * dt;
    c->pitch += dy * INPUT_LOOK_SPEED * dt;

    camera_update(c);
}

void camera_look_left(Camera *c, float dt) {
    camera_look(c, -1.0f, 0.0f, dt);
}

void camera_look_right(Camera *c, float dt) {
    camera_look(c, 1.0f, 0.0f, dt);
}

void camera_look_up(Camera *c, float dt) {
    camera_look(c, 0.0f, 1.0f, dt);
}

void camera_look_down(Camera *c, float dt) {
    camera_look(c, 0.0f, -1.0f, dt);
}

void camera_orbit_yaw(Camera *c, float d) {
    c->orbit_yaw += d;
    camera_update(c);
}

void camera_orbit_pitch(Camera *c, float d) {
    c->orbit_pitch = clampf(c->orbit_pitch + d, -1.4f, 1.4f);
    camera_update(c);
}

void camera_orbit_zoom(Camera *c, float d) {
    c->orbit_radius = clampf(c->orbit_radius + d, 1.0f, 50.0f);
    camera_update(c);
}

bool camera_project(const Camera *cam, Vec3 wp,
                    int *sx, int *sy, float *depth)
{

    Vec3 view = mat4_transform_point(cam->view, wp);

    if (view.z >= -cam->near_clip)
        return false;

    Vec3 clip = mat4_transform_point(cam->proj, view);

    float ndc_x = clip.x;
    float ndc_y = clip.y;
    float ndc_z = clip.z;

    if (ndc_x < -1.0f || ndc_x > 1.0f ||
        ndc_y < -1.0f || ndc_y > 1.0f ||
        ndc_z <  0.0f || ndc_z > 1.0f)
        return false;

    *sx = (int)((ndc_x * 0.5f + 0.5f) * SCREEN_WIDTH);
    *sy = (int)((-ndc_y * 0.5f + 0.5f) * SCREEN_HEIGHT);

    *depth = -view.z / cam->far_clip;

    return true;
}

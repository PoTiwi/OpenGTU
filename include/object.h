#ifndef OPENGTU_OBJECT_H
#define OPENGTU_OBJECT_H

#include <stdbool.h>
#include "math3d.h"
#include "graphics.h"

typedef enum {
    SHAPE_SPHERE    = 0,
    SHAPE_CUBE      = 1,
    SHAPE_CONE      = 2,
    SHAPE_CYLINDER  = 3,
    SHAPE_PLANE     = 4,
    SHAPE_CUSTOM    = 5,
} ShapeType;


typedef struct {
    Vec3 verts  [MAX_VERTS];
    int  vert_count;
    struct { int a, b, c; } faces[MAX_FACES];
    Vec3 normals[MAX_FACES];
    int  face_count;
} Mesh;

typedef struct {
    Vec3  velocity;
    Vec3  acceleration;
    float mass;
    bool  on_ground;
} PhysicsBody;

typedef struct {
    int       id;
    char      name[64];
    bool      active;

    Vec3      position;
    Vec3      rotation;
    Vec3      scale;

    bool      anchored;
    bool      collision;
    float     transparency;   /* 0 = opaque, 1 = invisible  */
    bool      morph;
    bool      wireframe;

    ShapeType shape;
    Mesh      mesh;

    int       voxels[16*16*16];
    int       vox_w, vox_h, vox_d;

    __structGS shading;

    PhysicsBody body;

    Mat4 world_matrix;
    bool dirty;
} Object;

typedef struct {
    Vec3  position;
    Vec3  direction;
    float intensity;
    bool  directional;
} Light;

typedef struct {
    Object objects[MAX_OBJECTS];
    int    obj_count;
    int    next_id;

    Light  lights[MAX_LIGHTS];
    int    light_count;

    Vec3   ambient;
    Vec3   gravity;
} Scene;

Object *scene_add_object   (Scene *s, ShapeType shape,
                             Vec3 pos, Vec3 rot, Vec3 scl);
void    scene_remove_object(Scene *s, int id);
Object *scene_find_object  (Scene *s, int id);
void    object_build_mesh  (Object *obj);
void    object_update_matrix(Object *obj);
float   object_light_at_face(Object *obj, int face, Scene *s);

void mesh_build_sphere  (Mesh *m, int slices, int stacks);
void mesh_build_cube    (Mesh *m);
void mesh_build_cone    (Mesh *m, int slices);
void mesh_build_cylinder(Mesh *m, int slices);
void mesh_build_plane   (Mesh *m, int divs);
void mesh_build_custom  (Mesh *m, const int *voxels,
                          int w, int h, int d, bool morph);

/*
 * 1 = filled voxel, 0 = empty
 * Pass to mesh_build_custom() with SHAPE_CUSTOM objects.
 *
 * Example:
 *   obj->vox_w = SHAPE_SQUARE_W;
 *   obj->vox_h = SHAPE_SQUARE_H;
 *   obj->vox_d = SHAPE_SQUARE_D;
 *   memcpy(obj->voxels, shape_square, sizeof(shape_square));
 *   object_build_mesh(obj);
 */
#define SHAPE_SQUARE_W  8
#define SHAPE_SQUARE_H 11
#define SHAPE_SQUARE_D  1

extern const int shape_square[];
extern const int shape_arch  [];
extern const int shape_cross [];

#endif /* OPENGTU_OBJECT_H */

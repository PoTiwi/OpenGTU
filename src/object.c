#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../include/object.h"
# define M_PI		3.14159265358979323846

const int shape_square[] = {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,   /* gap */
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1
};

const int shape_arch[] = {
    1,1,1,1,1,1,
    1,1,1,1,1,1,
    1,0,0,0,0,1,
    1,0,0,0,0,1,
    1,0,0,0,0,1,
    1,1,1,1,1,1
};

const int shape_cross[] = {
    0,0,1,0,0,
    0,0,1,0,0,
    1,1,1,1,1,
    0,0,1,0,0,
    0,0,1,0,0
};

void mesh_build_cube(Mesh *m) {

    static const float v[8][3] = {
        {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
        {-1,-1, 1},{1,-1, 1},{1,1, 1},{-1,1, 1}
    };

    static const int f[12][3] = {
        {0,1,2},{0,2,3}, {4,6,5},{4,7,6},
        {0,5,1},{0,4,5}, {2,6,7},{2,7,3},
        {0,3,7},{0,7,4}, {1,5,6},{1,6,2}
    };
    m->vert_count = 8;
    for (int i=0;i<8;i++) m->verts[i]=(Vec3){v[i][0],v[i][1],v[i][2]};
    m->face_count = 12;
    for (int i=0;i<12;i++){
        m->faces[i].a=f[i][0]; m->faces[i].b=f[i][1]; m->faces[i].c=f[i][2];
        Vec3 e1=v3_sub(m->verts[f[i][1]],m->verts[f[i][0]]);
        Vec3 e2=v3_sub(m->verts[f[i][2]],m->verts[f[i][0]]);
        m->normals[i]=v3_norm(v3_cross(e1,e2));
    }
}

#define FACE(m,fi,A,B,C) do{ (m)->faces[fi].a=(A); (m)->faces[fi].b=(B); (m)->faces[fi].c=(C); }while(0)

void mesh_build_sphere(Mesh *m, int slices, int stacks) {
    int vi=0, fi=0;
    for (int i=0;i<=stacks;i++) {
        float phi = (float)M_PI*i/stacks - (float)M_PI/2.0f;
        for (int j=0;j<=slices;j++) {
            float theta = 2.0f*(float)M_PI*j/slices;
            if (vi >= MAX_VERTS) break;
            m->verts[vi++]=(Vec3){
                cosf(phi)*cosf(theta),
                sinf(phi),
                cosf(phi)*sinf(theta)
            };
        }
    }
    m->vert_count = vi;
    for (int i=0;i<stacks&&fi+1<MAX_FACES;i++)
        for (int j=0;j<slices&&fi+1<MAX_FACES;j++) {
            int a=i*(slices+1)+j, b=a+1, c=a+(slices+1), d=c+1;
            FACE(m,fi,a,b,c); m->normals[fi]=v3_norm(m->verts[a]); fi++;
            FACE(m,fi,b,d,c); m->normals[fi]=v3_norm(m->verts[b]); fi++;
        }
    m->face_count = fi;
}

void mesh_build_cone(Mesh *m, int slices) {
    int vi=0, fi=0;
    m->verts[vi++]=(Vec3){0,1,0};
    m->verts[vi++]=(Vec3){0,-1,0};
    for (int i=0;i<slices;i++) {
        float a=2.0f*(float)M_PI*i/slices;
        m->verts[vi++]=(Vec3){cosf(a),-1,sinf(a)};
    }
    m->vert_count=vi;
    for (int i=0;i<slices&&fi+1<MAX_FACES;i++) {
        int c1=2+i, c2=2+(i+1)%slices;
        FACE(m,fi,0,c1,c2);
        Vec3 e1=v3_sub(m->verts[c1],m->verts[0]);
        Vec3 e2=v3_sub(m->verts[c2],m->verts[0]);
        m->normals[fi]=v3_norm(v3_cross(e1,e2)); fi++;
        FACE(m,fi,1,c2,c1); m->normals[fi]=(Vec3){0,-1,0}; fi++;
    }
    m->face_count=fi;
}

void mesh_build_cylinder(Mesh *m, int slices) {
    int vi=0, fi=0;
    m->verts[vi++]=(Vec3){0, 1,0};
    m->verts[vi++]=(Vec3){0,-1,0};
    for (int i=0;i<slices;i++){
        float a=2.0f*(float)M_PI*i/slices;
        m->verts[vi++]=(Vec3){cosf(a), 1,sinf(a)};
        m->verts[vi++]=(Vec3){cosf(a),-1,sinf(a)};
    }
    m->vert_count=vi;
    for (int i=0;i<slices&&fi+3<MAX_FACES;i++){
        int t1=2+2*i, b1=3+2*i, t2=2+2*((i+1)%slices), b2=3+2*((i+1)%slices);
        Vec3 n=v3_norm((Vec3){cosf(2.0f*(float)M_PI*i/slices),0,sinf(2.0f*(float)M_PI*i/slices)});
        FACE(m,fi,0,t1,t2);  m->normals[fi]=(Vec3){0,1,0};  fi++;
        FACE(m,fi,1,b2,b1);  m->normals[fi]=(Vec3){0,-1,0}; fi++;
        FACE(m,fi,t1,b1,t2); m->normals[fi]=n;               fi++;
        FACE(m,fi,b1,b2,t2); m->normals[fi]=n;               fi++;
    }
    m->face_count=fi;
}

void mesh_build_plane(Mesh *m, int d) {
    int vi=0, fi=0;
    for (int i=0;i<=d;i++) for (int j=0;j<=d;j++)
        m->verts[vi++]=(Vec3){(float)j/d*2-1, 0, (float)i/d*2-1};
    m->vert_count=vi;
    for (int i=0;i<d&&fi+1<MAX_FACES;i++) for (int j=0;j<d&&fi+1<MAX_FACES;j++) {
        int a=i*(d+1)+j;
        FACE(m,fi,a,a+1,a+d+2);   m->normals[fi]=(Vec3){0,1,0}; fi++;
        FACE(m,fi,a,a+d+2,a+d+1); m->normals[fi]=(Vec3){0,1,0}; fi++;
    }
    m->face_count=fi;
}

void mesh_build_custom(Mesh *m, const int *vox,
                        int w, int h, int d, bool morph) {
    (void)morph;
    int fi=0, vi=0;
    static const float cv[8][3]={{0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1}};
    static const int   cf[12][3]={{0,1,2},{0,2,3},{4,6,5},{4,7,6},{0,5,1},{0,4,5},{2,6,7},{2,7,3},{0,3,7},{0,7,4},{1,5,6},{1,6,2}};
    static const Vec3  cn[12]={{0,0,-1},{0,0,-1},{0,0,1},{0,0,1},{0,-1,0},{0,-1,0},{0,1,0},{0,1,0},{-1,0,0},{-1,0,0},{1,0,0},{1,0,0}};
    for (int iz=0;iz<d&&fi+11<MAX_FACES;iz++)
    for (int iy=0;iy<h&&fi+11<MAX_FACES;iy++)
    for (int ix=0;ix<w&&fi+11<MAX_FACES;ix++) {
        if (!vox[iz*h*w + iy*w + ix]) continue;
        float fx=(float)ix-(float)w/2.0f;
        float fy=(float)iy-(float)h/2.0f;
        float fz=(float)iz-(float)d/2.0f;
        int base=vi;
        for (int i=0;i<8&&vi<MAX_VERTS;i++)
            m->verts[vi++]=(Vec3){fx+cv[i][0],fy+cv[i][1],fz+cv[i][2]};
        for (int i=0;i<12&&fi<MAX_FACES;i++){
            FACE(m,fi,base+cf[i][0],base+cf[i][1],base+cf[i][2]);
            m->normals[fi]=cn[i]; fi++;
        }
    }
    m->vert_count=vi; m->face_count=fi;
}

void object_build_mesh(Object *obj) {
    memset(&obj->mesh, 0, sizeof(obj->mesh));
    switch (obj->shape) {
        case SHAPE_SPHERE:   mesh_build_sphere  (&obj->mesh, 16, 8); break;
        case SHAPE_CUBE:     mesh_build_cube    (&obj->mesh);        break;
        case SHAPE_CONE:     mesh_build_cone    (&obj->mesh, 16);    break;
        case SHAPE_CYLINDER: mesh_build_cylinder(&obj->mesh, 16);    break;
        case SHAPE_PLANE:    mesh_build_plane   (&obj->mesh, 8);     break;
        case SHAPE_CUSTOM:
            mesh_build_custom(&obj->mesh, obj->voxels,
                              obj->vox_w, obj->vox_h, obj->vox_d,
                              obj->morph);
            break;
    }
    obj->dirty = false;
}

void object_update_matrix(Object *obj) {
    Mat4 T = mat4_translate(obj->position.x,obj->position.y,obj->position.z);
    Mat4 Rx= mat4_rotate_x(obj->rotation.x);
    Mat4 Ry= mat4_rotate_y(obj->rotation.y);
    Mat4 Rz= mat4_rotate_z(obj->rotation.z);
    Mat4 S = mat4_scale(obj->scale.x,obj->scale.y,obj->scale.z);
    obj->world_matrix = mat4_mul(T, mat4_mul(Ry, mat4_mul(Rx, mat4_mul(Rz, S))));
}

float object_light_at_face(Object *obj, int face, Scene *s) {
    if (face < 0 || face >= obj->mesh.face_count) return 0.5f;
    Vec3 wn = mat4_transform_dir(obj->world_matrix, obj->mesh.normals[face]);
    wn = v3_norm(wn);
    float light = v3_dot((Vec3){s->ambient.x,s->ambient.y,s->ambient.z},
                         (Vec3){1,1,1}) / 3.0f;
    for (int i = 0; i < s->light_count; i++) {
        Vec3 ld = s->lights[i].directional
                ? v3_norm(s->lights[i].direction)
                : v3_norm(v3_sub(s->lights[i].position,
                          mat4_transform_point(obj->world_matrix,(Vec3){0,0,0})));
        float d = v3_dot(wn, ld);
        if (d > 0) light += d * s->lights[i].intensity;
    }
    return CLAMP(light, 0.0f, 1.0f);
}

Object *scene_add_object(Scene *s, ShapeType shape,
                          Vec3 pos, Vec3 rot, Vec3 scl) {
    if (s->obj_count >= MAX_OBJECTS) return NULL;
    Object *obj = &s->objects[s->obj_count++];
    memset(obj, 0, sizeof(*obj));
    obj->id          = s->next_id++;
    obj->active      = true;
    obj->shape       = shape;
    obj->position    = pos;
    obj->rotation    = rot;
    obj->scale       = scl;
    obj->anchored    = false;
    obj->collision   = true;
    obj->transparency= 0.0f;
    obj->morph       = false;
    obj->wireframe   = false;
    obj->shading     = __retu_gst();
    obj->body.mass   = 1.0f;
    snprintf(obj->name, sizeof(obj->name), "Object_%d", obj->id);
    object_build_mesh(obj);
    object_update_matrix(obj);
    return obj;
}

void scene_remove_object(Scene *s, int id) {
    for (int i = 0; i < s->obj_count; i++) {
        if (s->objects[i].id == id) {
            s->objects[i] = s->objects[--s->obj_count];
            return;
        }
    }
}

Object *scene_find_object(Scene *s, int id) {
    for (int i = 0; i < s->obj_count; i++)
        if (s->objects[i].id == id) return &s->objects[i];
    return NULL;
}

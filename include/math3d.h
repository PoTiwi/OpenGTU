#ifndef OPENGTU_MATH3D_H
#define OPENGTU_MATH3D_H

#include <math.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct { float x, y, z; } Vec3;
typedef struct { float x, y;    } Vec2;
typedef struct { float m[4][4]; } Mat4;

static inline Vec3  v3_add  (Vec3 a, Vec3 b){ return (Vec3){a.x+b.x,a.y+b.y,a.z+b.z}; }
static inline Vec3  v3_sub  (Vec3 a, Vec3 b){ return (Vec3){a.x-b.x,a.y-b.y,a.z-b.z}; }
static inline Vec3  v3_scale(Vec3 a, float s){ return (Vec3){a.x*s,a.y*s,a.z*s}; }
static inline float v3_dot  (Vec3 a, Vec3 b){ return a.x*b.x+a.y*b.y+a.z*b.z; }
static inline float v3_len  (Vec3 a){ return sqrtf(a.x*a.x+a.y*a.y+a.z*a.z); }
static inline Vec3  v3_norm (Vec3 a){
    float l = v3_len(a);
    return l < 1e-6f ? (Vec3){0,0,0} : v3_scale(a, 1.0f/l);
}
static inline Vec3 v3_cross(Vec3 a, Vec3 b){
    return (Vec3){ a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x };
}
static inline Vec3 v3_lerp(Vec3 a, Vec3 b, float t){
    return v3_add(a, v3_scale(v3_sub(b,a), t));
}
static inline Vec3 v3_zero(void){ return (Vec3){0,0,0}; }
static inline Vec3 v3_one (void){ return (Vec3){1,1,1}; }

Mat4  mat4_identity  (void);
Mat4  mat4_mul       (Mat4 a, Mat4 b);
Vec3  mat4_transform_point(Mat4 m, Vec3 v);
Vec3  mat4_transform_dir  (Mat4 m, Vec3 v);
Mat4  mat4_translate (float x, float y, float z);
Mat4  mat4_scale     (float x, float y, float z);
Mat4  mat4_rotate_x  (float rad);
Mat4  mat4_rotate_y  (float rad);
Mat4  mat4_rotate_z  (float rad);
Mat4  mat4_perspective(float fov_deg, float aspect, float near, float far);
Mat4  mat4_look_at   (Vec3 eye, Vec3 target, Vec3 up);

#define DEG2RAD(d)      ((d) * (float)M_PI / 180.0f)
#define CLAMP(v,lo,hi)  ((v)<(lo)?(lo):(v)>(hi)?(hi):(v))
#define GTU_MAX(a,b)    ((a)>(b)?(a):(b))
#define GTU_MIN(a,b)    ((a)<(b)?(a):(b))

#endif /* OPENGTU_MATH3D_H */

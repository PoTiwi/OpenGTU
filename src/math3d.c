#include <math.h>
#include <string.h>
#include "../include/math3d.h"

Mat4 mat4_identity(void) {
    Mat4 m; memset(&m, 0, sizeof(m));
    m.m[0][0]=m.m[1][1]=m.m[2][2]=m.m[3][3]=1.0f;
    return m;
}

Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 r; memset(&r, 0, sizeof(r));
    for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
            for (int k=0;k<4;k++)
                r.m[i][j] += a.m[i][k] * b.m[k][j];
    return r;
}

Vec3 mat4_transform_point(Mat4 m, Vec3 v) {
    float w = m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3];
    return (Vec3){
        (m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]) / w,
        (m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]) / w,
        (m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]) / w
    };
}

Vec3 mat4_transform_dir(Mat4 m, Vec3 v) {
    return (Vec3){
        m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z,
        m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z,
        m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z
    };
}

Mat4 mat4_translate(float x, float y, float z) {
    Mat4 m = mat4_identity();
    m.m[0][3]=x; m.m[1][3]=y; m.m[2][3]=z;
    return m;
}

Mat4 mat4_scale(float x, float y, float z) {
    Mat4 m = mat4_identity();
    m.m[0][0]=x; m.m[1][1]=y; m.m[2][2]=z;
    return m;
}

Mat4 mat4_rotate_x(float r) {
    Mat4 m = mat4_identity();
    m.m[1][1]= cosf(r); m.m[1][2]=-sinf(r);
    m.m[2][1]= sinf(r); m.m[2][2]= cosf(r);
    return m;
}

Mat4 mat4_rotate_y(float r) {
    Mat4 m = mat4_identity();
    m.m[0][0]= cosf(r); m.m[0][2]= sinf(r);
    m.m[2][0]=-sinf(r); m.m[2][2]= cosf(r);
    return m;
}

Mat4 mat4_rotate_z(float r) {
    Mat4 m = mat4_identity();
    m.m[0][0]= cosf(r); m.m[0][1]=-sinf(r);
    m.m[1][0]= sinf(r); m.m[1][1]= cosf(r);
    return m;
}

Mat4 mat4_perspective(float fov_deg, float aspect, float near, float far) {
    Mat4 m; memset(&m, 0, sizeof(m));
    float f = 1.0f / tanf(DEG2RAD(fov_deg) * 0.5f);
    m.m[0][0] = f / aspect;
    m.m[1][1] = f;
    m.m[2][2] = (far + near) / (near - far);
    m.m[2][3] = (2.0f * far * near) / (near - far);
    m.m[3][2] = -1.0f;
    return m;
}

Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 f = v3_norm(v3_sub(target, eye));
    Vec3 r = v3_norm(v3_cross(f, up));
    Vec3 u = v3_cross(r, f);
    Mat4 m = mat4_identity();
    m.m[0][0]=r.x; m.m[0][1]=r.y; m.m[0][2]=r.z; m.m[0][3]=-v3_dot(r,eye);
    m.m[1][0]=u.x; m.m[1][1]=u.y; m.m[1][2]=u.z; m.m[1][3]=-v3_dot(u,eye);
    m.m[2][0]=-f.x;m.m[2][1]=-f.y;m.m[2][2]=-f.z;m.m[2][3]= v3_dot(f,eye);
    return m;
}

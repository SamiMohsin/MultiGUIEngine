/**
 * @file mg_mat4.c
 * @brief 4x4 matrix operations implementation (row-major, Vulkan Y-down depth [0, 1]).
 */
#include "mg/math/mat4.h"
#include "mg/math/vec3.h"
#include "mg/math/vec4.h"
#include <math.h>
#include <string.h>

void mg_mat4_identity(mg_mat4_t* out) {
    if (!out) return;
    memset(out->m, 0, sizeof(float) * 16);
    out->m[0] = 1.0f;
    out->m[5] = 1.0f;
    out->m[10] = 1.0f;
    out->m[15] = 1.0f;
}

void mg_mat4_zero(mg_mat4_t* out) {
    if (!out) return;
    memset(out->m, 0, sizeof(float) * 16);
}

void mg_mat4_copy(mg_mat4_t* out, const mg_mat4_t* src) {
    if (!out || !src) return;
    memcpy(out->m, src->m, sizeof(float) * 16);
}

void mg_mat4_mul(mg_mat4_t* out, const mg_mat4_t* a, const mg_mat4_t* b) {
    if (!out || !a || !b) return;
    mg_mat4_t tmp;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            tmp.m[r * 4 + c] = 
                a->m[r * 4 + 0] * b->m[0 * 4 + c] +
                a->m[r * 4 + 1] * b->m[1 * 4 + c] +
                a->m[r * 4 + 2] * b->m[2 * 4 + c] +
                a->m[r * 4 + 3] * b->m[3 * 4 + c];
        }
    }
    memcpy(out->m, tmp.m, sizeof(float) * 16);
}

void mg_mat4_transform_vec4(mg_vec4_t* out, const mg_mat4_t* m, const mg_vec4_t* v) {
    if (!out || !m || !v) return;
    float x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z + m->m[3] * v->w;
    float y = m->m[4] * v->x + m->m[5] * v->y + m->m[6] * v->z + m->m[7] * v->w;
    float z = m->m[8] * v->x + m->m[9] * v->y + m->m[10] * v->z + m->m[11] * v->w;
    float w = m->m[12] * v->x + m->m[13] * v->y + m->m[14] * v->z + m->m[15] * v->w;
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
}

void mg_mat4_transform_point(mg_vec3_t* out, const mg_mat4_t* m, const mg_vec3_t* v) {
    if (!out || !m || !v) return;
    float x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z + m->m[3];
    float y = m->m[4] * v->x + m->m[5] * v->y + m->m[6] * v->z + m->m[7];
    float z = m->m[8] * v->x + m->m[9] * v->y + m->m[10] * v->z + m->m[11];
    float w = m->m[12] * v->x + m->m[13] * v->y + m->m[14] * v->z + m->m[15];
    if (fabsf(w) > 1e-6f) {
        float inv_w = 1.0f / w;
        out->x = x * inv_w;
        out->y = y * inv_w;
        out->z = z * inv_w;
    } else {
        out->x = x;
        out->y = y;
        out->z = z;
    }
}

void mg_mat4_transform_vector(mg_vec3_t* out, const mg_mat4_t* m, const mg_vec3_t* v) {
    if (!out || !m || !v) return;
    out->x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z;
    out->y = m->m[4] * v->x + m->m[5] * v->y + m->m[6] * v->z;
    out->z = m->m[8] * v->x + m->m[9] * v->y + m->m[10] * v->z;
}

void mg_mat4_transpose(mg_mat4_t* out, const mg_mat4_t* a) {
    if (!out || !a) return;
    mg_mat4_t tmp;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            tmp.m[r * 4 + c] = a->m[c * 4 + r];
        }
    }
    memcpy(out->m, tmp.m, sizeof(float) * 16);
}

bool mg_mat4_invert(mg_mat4_t* out, const mg_mat4_t* a) {
    if (!out || !a) return false;
    const float* m = a->m;
    float inv[16];

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    if (fabsf(det) < 1e-8f) {
        return false;
    }

    float inv_det = 1.0f / det;
    for (int i = 0; i < 16; ++i) {
        out->m[i] = inv[i] * inv_det;
    }
    return true;
}

void mg_mat4_translation(mg_mat4_t* out, const mg_vec3_t* translation) {
    if (!out) return;
    mg_mat4_identity(out);
    if (translation) {
        out->m[3] = translation->x;
        out->m[7] = translation->y;
        out->m[11] = translation->z;
    }
}

void mg_mat4_scaling(mg_mat4_t* out, const mg_vec3_t* scale) {
    if (!out) return;
    mg_mat4_identity(out);
    if (scale) {
        out->m[0] = scale->x;
        out->m[5] = scale->y;
        out->m[10] = scale->z;
    }
}

void mg_mat4_rotation_axis(mg_mat4_t* out, const mg_vec3_t* axis, float angle_rad) {
    if (!out || !axis) return;
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    float t = 1.0f - c;

    mg_vec3_t a;
    mg_vec3_normalize(&a, axis);

    out->m[0] = t * a.x * a.x + c;
    out->m[1] = t * a.x * a.y - s * a.z;
    out->m[2] = t * a.x * a.z + s * a.y;
    out->m[3] = 0.0f;

    out->m[4] = t * a.x * a.y + s * a.z;
    out->m[5] = t * a.y * a.y + c;
    out->m[6] = t * a.y * a.z - s * a.x;
    out->m[7] = 0.0f;

    out->m[8]  = t * a.x * a.z - s * a.y;
    out->m[9]  = t * a.y * a.z + s * a.x;
    out->m[10] = t * a.z * a.z + c;
    out->m[11] = 0.0f;

    out->m[12] = 0.0f;
    out->m[13] = 0.0f;
    out->m[14] = 0.0f;
    out->m[15] = 1.0f;
}

void mg_mat4_rotation_euler(mg_mat4_t* out, float pitch, float yaw, float roll) {
    if (!out) return;
    float cp = cosf(pitch), sp = sinf(pitch);
    float cy = cosf(yaw), sy = sinf(yaw);
    float cr = cosf(roll), sr = sinf(roll);

    out->m[0] = cy * cr + sy * sp * sr;
    out->m[1] = -cy * sr + sy * sp * cr;
    out->m[2] = sy * cp;
    out->m[3] = 0.0f;

    out->m[4] = cp * sr;
    out->m[5] = cp * cr;
    out->m[6] = -sp;
    out->m[7] = 0.0f;

    out->m[8]  = -sy * cr + cy * sp * sr;
    out->m[9]  = sy * sr + cy * sp * cr;
    out->m[10] = cy * cp;
    out->m[11] = 0.0f;

    out->m[12] = 0.0f;
    out->m[13] = 0.0f;
    out->m[14] = 0.0f;
    out->m[15] = 1.0f;
}

void mg_mat4_perspective(mg_mat4_t* out, float fov_y_rad, float aspect, float z_near, float z_far) {
    if (!out) return;
    mg_mat4_zero(out);
    float tan_half_fov = tanf(fov_y_rad * 0.5f);
    if (fabsf(tan_half_fov) < 1e-6f || fabsf(aspect) < 1e-6f || fabsf(z_far - z_near) < 1e-6f) {
        return;
    }

    out->m[0] = 1.0f / (aspect * tan_half_fov);
    out->m[5] = -1.0f / tan_half_fov; /* Vulkan Y-down */
    out->m[10] = z_far / (z_near - z_far);
    out->m[11] = (z_near * z_far) / (z_near - z_far);
    out->m[14] = -1.0f;
}

void mg_mat4_ortho(mg_mat4_t* out, float left, float right, float bottom, float top, float z_near, float z_far) {
    if (!out) return;
    mg_mat4_zero(out);
    float rl = right - left;
    float tb = bottom - top; /* Y-down */
    float fn = z_far - z_near;

    if (fabsf(rl) < 1e-6f || fabsf(tb) < 1e-6f || fabsf(fn) < 1e-6f) {
        return;
    }

    out->m[0]  = 2.0f / rl;
    out->m[3]  = -(right + left) / rl;
    out->m[5]  = -2.0f / tb;
    out->m[7]  = -(bottom + top) / tb;
    out->m[10] = -1.0f / fn;
    out->m[11] = -z_near / fn;
    out->m[15] = 1.0f;
}

void mg_mat4_look_at(mg_mat4_t* out, const mg_vec3_t* eye, const mg_vec3_t* target, const mg_vec3_t* up) {
    if (!out || !eye || !target || !up) return;
    mg_vec3_t f, r, u;

    /* forward = target - eye */
    mg_vec3_sub(&f, target, eye);
    mg_vec3_normalize(&f, &f);

    /* right = cross(forward, up) */
    mg_vec3_cross(&r, &f, up);
    mg_vec3_normalize(&r, &r);

    /* true up = cross(right, forward) */
    mg_vec3_cross(&u, &r, &f);

    out->m[0] = r.x;
    out->m[1] = r.y;
    out->m[2] = r.z;
    out->m[3] = -mg_vec3_dot(&r, eye);

    out->m[4] = u.x;
    out->m[5] = u.y;
    out->m[6] = u.z;
    out->m[7] = -mg_vec3_dot(&u, eye);

    out->m[8]  = -f.x;
    out->m[9]  = -f.y;
    out->m[10] = -f.z;
    out->m[11] = mg_vec3_dot(&f, eye);

    out->m[12] = 0.0f;
    out->m[13] = 0.0f;
    out->m[14] = 0.0f;
    out->m[15] = 1.0f;
}

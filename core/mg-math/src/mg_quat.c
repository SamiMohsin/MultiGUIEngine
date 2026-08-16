/**
 * @file mg_quat.c
 * @brief Quaternion operations implementation.
 */
#include "mg/math/quat.h"
#include <math.h>

void mg_quat_set(mg_quat_t* out, float x, float y, float z, float w) {
    if (!out) return;
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
}

void mg_quat_identity(mg_quat_t* out) {
    if (!out) return;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    out->w = 1.0f;
}

void mg_quat_copy(mg_quat_t* out, const mg_quat_t* src) {
    if (!out || !src) return;
    out->x = src->x;
    out->y = src->y;
    out->z = src->z;
    out->w = src->w;
}

void mg_quat_from_axis_angle(mg_quat_t* out, const mg_vec3_t* axis, float angle_rad) {
    if (!out || !axis) return;
    float half_angle = angle_rad * 0.5f;
    float s = sinf(half_angle);
    mg_vec3_t norm_axis;
    mg_vec3_normalize(&norm_axis, axis);

    out->x = norm_axis.x * s;
    out->y = norm_axis.y * s;
    out->z = norm_axis.z * s;
    out->w = cosf(half_angle);
}

void mg_quat_from_euler(mg_quat_t* out, float pitch, float yaw, float roll) {
    if (!out) return;
    float half_p = pitch * 0.5f;
    float half_y = yaw * 0.5f;
    float half_r = roll * 0.5f;

    float cp = cosf(half_p), sp = sinf(half_p);
    float cy = cosf(half_y), sy = sinf(half_y);
    float cr = cosf(half_r), sr = sinf(half_r);

    out->x = sp * cy * cr - cp * sy * sr;
    out->y = cp * sy * cr + sp * cy * sr;
    out->z = cp * cy * sr - sp * sy * cr;
    out->w = cp * cy * cr + sp * sy * sr;
}

void mg_quat_mul(mg_quat_t* out, const mg_quat_t* a, const mg_quat_t* b) {
    if (!out || !a || !b) return;
    float x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    float y = a->w * b->y - a->x * b->z + a->y * b->w + a->z * b->x;
    float z = a->w * b->z + a->x * b->y - a->y * b->x + a->z * b->w;
    float w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
}

float mg_quat_dot(const mg_quat_t* a, const mg_quat_t* b) {
    if (!a || !b) return 0.0f;
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

float mg_quat_length_sqr(const mg_quat_t* q) {
    if (!q) return 0.0f;
    return q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;
}

float mg_quat_length(const mg_quat_t* q) {
    if (!q) return 0.0f;
    return sqrtf(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
}

void mg_quat_normalize(mg_quat_t* out, const mg_quat_t* q) {
    if (!out || !q) return;
    float len = sqrtf(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
    if (len > 1e-6f) {
        float inv = 1.0f / len;
        out->x = q->x * inv;
        out->y = q->y * inv;
        out->z = q->z * inv;
        out->w = q->w * inv;
    } else {
        mg_quat_identity(out);
    }
}

void mg_quat_conjugate(mg_quat_t* out, const mg_quat_t* q) {
    if (!out || !q) return;
    out->x = -q->x;
    out->y = -q->y;
    out->z = -q->z;
    out->w = q->w;
}

void mg_quat_slerp(mg_quat_t* out, const mg_quat_t* a, const mg_quat_t* b, float t) {
    if (!out || !a || !b) return;
    float cos_half_theta = mg_quat_dot(a, b);
    mg_quat_t target = *b;

    if (cos_half_theta < 0.0f) {
        target.x = -target.x;
        target.y = -target.y;
        target.z = -target.z;
        target.w = -target.w;
        cos_half_theta = -cos_half_theta;
    }

    if (cos_half_theta >= 1.0f - 1e-5f) {
        /* Very close: linear interpolation */
        out->x = a->x + t * (target.x - a->x);
        out->y = a->y + t * (target.y - a->y);
        out->z = a->z + t * (target.z - a->z);
        out->w = a->w + t * (target.w - a->w);
        mg_quat_normalize(out, out);
        return;
    }

    float half_theta = acosf(cos_half_theta);
    float sin_half_theta = sqrtf(1.0f - cos_half_theta * cos_half_theta);
    if (fabsf(sin_half_theta) < 1e-6f) {
        *out = *a;
        return;
    }

    float ratio_a = sinf((1.0f - t) * half_theta) / sin_half_theta;
    float ratio_b = sinf(t * half_theta) / sin_half_theta;

    out->x = a->x * ratio_a + target.x * ratio_b;
    out->y = a->y * ratio_a + target.y * ratio_b;
    out->z = a->z * ratio_a + target.z * ratio_b;
    out->w = a->w * ratio_a + target.w * ratio_b;
}

void mg_quat_to_mat4(mg_mat4_t* out, const mg_quat_t* q) {
    if (!out || !q) return;
    mg_mat4_identity(out);

    float xx = q->x * q->x;
    float yy = q->y * q->y;
    float zz = q->z * q->z;
    float xy = q->x * q->y;
    float xz = q->x * q->z;
    float yz = q->y * q->z;
    float wx = q->w * q->x;
    float wy = q->w * q->y;
    float wz = q->w * q->z;

    out->m[0] = 1.0f - 2.0f * (yy + zz);
    out->m[1] = 2.0f * (xy - wz);
    out->m[2] = 2.0f * (xz + wy);

    out->m[4] = 2.0f * (xy + wz);
    out->m[5] = 1.0f - 2.0f * (xx + zz);
    out->m[6] = 2.0f * (yz - wx);

    out->m[8]  = 2.0f * (xz - wy);
    out->m[9]  = 2.0f * (yz + wx);
    out->m[10] = 1.0f - 2.0f * (xx + yy);
}

void mg_quat_rotate_vec3(mg_vec3_t* out, const mg_quat_t* q, const mg_vec3_t* v) {
    if (!out || !q || !v) return;
    /* v' = 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v) + v */
    mg_vec3_t qv = { q->x, q->y, q->z };
    mg_vec3_t uv, uuv;

    mg_vec3_cross(&uv, &qv, v);
    mg_vec3_cross(&uuv, &qv, &uv);

    mg_vec3_scale(&uv, &uv, q->w * 2.0f);
    mg_vec3_scale(&uuv, &uuv, 2.0f);

    mg_vec3_add(out, v, &uv);
    mg_vec3_add(out, out, &uuv);
}

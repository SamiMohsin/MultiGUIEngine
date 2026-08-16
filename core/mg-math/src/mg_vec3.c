/**
 * @file mg_vec3.c
 * @brief 3D vector operations implementation.
 */
#include "mg/math/vec3.h"
#include <math.h>

void mg_vec3_set(mg_vec3_t* out, float x, float y, float z) {
    if (!out) return;
    out->x = x;
    out->y = y;
    out->z = z;
}

void mg_vec3_zero(mg_vec3_t* out) {
    if (!out) return;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
}

void mg_vec3_copy(mg_vec3_t* out, const mg_vec3_t* src) {
    if (!out || !src) return;
    out->x = src->x;
    out->y = src->y;
    out->z = src->z;
}

void mg_vec3_add(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void mg_vec3_sub(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void mg_vec3_scale(mg_vec3_t* out, const mg_vec3_t* a, float scalar) {
    if (!out || !a) return;
    out->x = a->x * scalar;
    out->y = a->y * scalar;
    out->z = a->z * scalar;
}

void mg_vec3_mul(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x * b->x;
    out->y = a->y * b->y;
    out->z = a->z * b->z;
}

float mg_vec3_dot(const mg_vec3_t* a, const mg_vec3_t* b) {
    if (!a || !b) return 0.0f;
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void mg_vec3_cross(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b) {
    if (!out || !a || !b) return;
    float x = a->y * b->z - a->z * b->y;
    float y = a->z * b->x - a->x * b->z;
    float z = a->x * b->y - a->y * b->x;
    out->x = x;
    out->y = y;
    out->z = z;
}

float mg_vec3_length_sqr(const mg_vec3_t* v) {
    if (!v) return 0.0f;
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

float mg_vec3_length(const mg_vec3_t* v) {
    if (!v) return 0.0f;
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

void mg_vec3_normalize(mg_vec3_t* out, const mg_vec3_t* v) {
    if (!out || !v) return;
    float len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len > 1e-6f) {
        float inv = 1.0f / len;
        out->x = v->x * inv;
        out->y = v->y * inv;
        out->z = v->z * inv;
    } else {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
    }
}

float mg_vec3_distance(const mg_vec3_t* a, const mg_vec3_t* b) {
    if (!a || !b) return 0.0f;
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

void mg_vec3_lerp(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b, float t) {
    if (!out || !a || !b) return;
    out->x = a->x + t * (b->x - a->x);
    out->y = a->y + t * (b->y - a->y);
    out->z = a->z + t * (b->z - a->z);
}

/**
 * @file mg_vec4.c
 * @brief 4D vector operations implementation.
 */
#include "mg/math/vec4.h"
#include <math.h>

void mg_vec4_set(mg_vec4_t* out, float x, float y, float z, float w) {
    if (!out) return;
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
}

void mg_vec4_zero(mg_vec4_t* out) {
    if (!out) return;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    out->w = 0.0f;
}

void mg_vec4_copy(mg_vec4_t* out, const mg_vec4_t* src) {
    if (!out || !src) return;
    out->x = src->x;
    out->y = src->y;
    out->z = src->z;
    out->w = src->w;
}

void mg_vec4_add(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
    out->w = a->w + b->w;
}

void mg_vec4_sub(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
    out->w = a->w - b->w;
}

void mg_vec4_scale(mg_vec4_t* out, const mg_vec4_t* a, float scalar) {
    if (!out || !a) return;
    out->x = a->x * scalar;
    out->y = a->y * scalar;
    out->z = a->z * scalar;
    out->w = a->w * scalar;
}

void mg_vec4_mul(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x * b->x;
    out->y = a->y * b->y;
    out->z = a->z * b->z;
    out->w = a->w * b->w;
}

float mg_vec4_dot(const mg_vec4_t* a, const mg_vec4_t* b) {
    if (!a || !b) return 0.0f;
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

float mg_vec4_length_sqr(const mg_vec4_t* v) {
    if (!v) return 0.0f;
    return v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w;
}

float mg_vec4_length(const mg_vec4_t* v) {
    if (!v) return 0.0f;
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
}

void mg_vec4_normalize(mg_vec4_t* out, const mg_vec4_t* v) {
    if (!out || !v) return;
    float len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
    if (len > 1e-6f) {
        float inv = 1.0f / len;
        out->x = v->x * inv;
        out->y = v->y * inv;
        out->z = v->z * inv;
        out->w = v->w * inv;
    } else {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
        out->w = 0.0f;
    }
}

void mg_vec4_lerp(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b, float t) {
    if (!out || !a || !b) return;
    out->x = a->x + t * (b->x - a->x);
    out->y = a->y + t * (b->y - a->y);
    out->z = a->z + t * (b->z - a->z);
    out->w = a->w + t * (b->w - a->w);
}

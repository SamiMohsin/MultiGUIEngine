/**
 * @file mg_vec2.c
 * @brief 2D vector operations implementation.
 */
#include "mg/math/vec2.h"
#include <math.h>

void mg_vec2_set(mg_vec2_t* out, float x, float y) {
    if (!out) return;
    out->x = x;
    out->y = y;
}

void mg_vec2_zero(mg_vec2_t* out) {
    if (!out) return;
    out->x = 0.0f;
    out->y = 0.0f;
}

void mg_vec2_copy(mg_vec2_t* out, const mg_vec2_t* src) {
    if (!out || !src) return;
    out->x = src->x;
    out->y = src->y;
}

void mg_vec2_add(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

void mg_vec2_sub(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x - b->x;
    out->y = a->y - b->y;
}

void mg_vec2_scale(mg_vec2_t* out, const mg_vec2_t* a, float scalar) {
    if (!out || !a) return;
    out->x = a->x * scalar;
    out->y = a->y * scalar;
}

void mg_vec2_mul(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b) {
    if (!out || !a || !b) return;
    out->x = a->x * b->x;
    out->y = a->y * b->y;
}

float mg_vec2_dot(const mg_vec2_t* a, const mg_vec2_t* b) {
    if (!a || !b) return 0.0f;
    return a->x * b->x + a->y * b->y;
}

float mg_vec2_length_sqr(const mg_vec2_t* v) {
    if (!v) return 0.0f;
    return v->x * v->x + v->y * v->y;
}

float mg_vec2_length(const mg_vec2_t* v) {
    if (!v) return 0.0f;
    return sqrtf(v->x * v->x + v->y * v->y);
}

void mg_vec2_normalize(mg_vec2_t* out, const mg_vec2_t* v) {
    if (!out || !v) return;
    float len = sqrtf(v->x * v->x + v->y * v->y);
    if (len > 1e-6f) {
        float inv = 1.0f / len;
        out->x = v->x * inv;
        out->y = v->y * inv;
    } else {
        out->x = 0.0f;
        out->y = 0.0f;
    }
}

float mg_vec2_distance(const mg_vec2_t* a, const mg_vec2_t* b) {
    if (!a || !b) return 0.0f;
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    return sqrtf(dx * dx + dy * dy);
}

void mg_vec2_lerp(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b, float t) {
    if (!out || !a || !b) return;
    out->x = a->x + t * (b->x - a->x);
    out->y = a->y + t * (b->y - a->y);
}

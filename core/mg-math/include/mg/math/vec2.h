/**
 * @file vec2.h
 * @brief 2D vector operations.
 */
#ifndef MG_MATH_VEC2_H
#define MG_MATH_VEC2_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 2-component floating-point vector.
 */
typedef struct mg_vec2 {
    float x;
    float y;
} mg_vec2_t;

/** Sets vector components directly. */
void mg_vec2_set(mg_vec2_t* out, float x, float y);

/** Sets all components to zero. */
void mg_vec2_zero(mg_vec2_t* out);

/** Copies vector src to out. */
void mg_vec2_copy(mg_vec2_t* out, const mg_vec2_t* src);

/** Adds two vectors: out = a + b. */
void mg_vec2_add(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b);

/** Subtracts two vectors: out = a - b. */
void mg_vec2_sub(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b);

/** Multiplies vector by scalar: out = a * scalar. */
void mg_vec2_scale(mg_vec2_t* out, const mg_vec2_t* a, float scalar);

/** Component-wise multiplication: out = a * b. */
void mg_vec2_mul(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b);

/** Computes dot product: a . b. */
float mg_vec2_dot(const mg_vec2_t* a, const mg_vec2_t* b);

/** Computes length squared of vector. */
float mg_vec2_length_sqr(const mg_vec2_t* v);

/** Computes length (magnitude) of vector. */
float mg_vec2_length(const mg_vec2_t* v);

/** Normalizes vector to unit length. */
void mg_vec2_normalize(mg_vec2_t* out, const mg_vec2_t* v);

/** Computes distance between two points. */
float mg_vec2_distance(const mg_vec2_t* a, const mg_vec2_t* b);

/** Computes linear interpolation between two vectors: out = a + t * (b - a). */
void mg_vec2_lerp(mg_vec2_t* out, const mg_vec2_t* a, const mg_vec2_t* b, float t);

#ifdef __cplusplus
}
#endif

#endif /* MG_MATH_VEC2_H */

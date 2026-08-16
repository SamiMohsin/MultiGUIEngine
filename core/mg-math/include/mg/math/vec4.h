/**
 * @file vec4.h
 * @brief 4D vector operations.
 */
#ifndef MG_MATH_VEC4_H
#define MG_MATH_VEC4_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 4-component floating-point vector.
 */
typedef struct mg_vec4 {
    float x;
    float y;
    float z;
    float w;
} mg_vec4_t;

/** Sets vector components directly. */
void mg_vec4_set(mg_vec4_t* out, float x, float y, float z, float w);

/** Sets all components to zero. */
void mg_vec4_zero(mg_vec4_t* out);

/** Copies vector src to out. */
void mg_vec4_copy(mg_vec4_t* out, const mg_vec4_t* src);

/** Adds two vectors: out = a + b. */
void mg_vec4_add(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b);

/** Subtracts two vectors: out = a - b. */
void mg_vec4_sub(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b);

/** Multiplies vector by scalar: out = a * scalar. */
void mg_vec4_scale(mg_vec4_t* out, const mg_vec4_t* a, float scalar);

/** Component-wise multiplication: out = a * b. */
void mg_vec4_mul(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b);

/** Computes dot product: a . b. */
float mg_vec4_dot(const mg_vec4_t* a, const mg_vec4_t* b);

/** Computes length squared of vector. */
float mg_vec4_length_sqr(const mg_vec4_t* v);

/** Computes length (magnitude) of vector. */
float mg_vec4_length(const mg_vec4_t* v);

/** Normalizes vector to unit length. */
void mg_vec4_normalize(mg_vec4_t* out, const mg_vec4_t* v);

/** Computes linear interpolation between two vectors: out = a + t * (b - a). */
void mg_vec4_lerp(mg_vec4_t* out, const mg_vec4_t* a, const mg_vec4_t* b, float t);

#ifdef __cplusplus
}
#endif

#endif /* MG_MATH_VEC4_H */

/**
 * @file vec3.h
 * @brief 3D vector operations.
 */
#ifndef MG_MATH_VEC3_H
#define MG_MATH_VEC3_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 3-component floating-point vector.
 */
typedef struct mg_vec3 {
    float x;
    float y;
    float z;
} mg_vec3_t;

/** Sets vector components directly. */
void mg_vec3_set(mg_vec3_t* out, float x, float y, float z);

/** Sets all components to zero. */
void mg_vec3_zero(mg_vec3_t* out);

/** Copies vector src to out. */
void mg_vec3_copy(mg_vec3_t* out, const mg_vec3_t* src);

/** Adds two vectors: out = a + b. */
void mg_vec3_add(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b);

/** Subtracts two vectors: out = a - b. */
void mg_vec3_sub(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b);

/** Multiplies vector by scalar: out = a * scalar. */
void mg_vec3_scale(mg_vec3_t* out, const mg_vec3_t* a, float scalar);

/** Component-wise multiplication: out = a * b. */
void mg_vec3_mul(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b);

/** Computes dot product: a . b. */
float mg_vec3_dot(const mg_vec3_t* a, const mg_vec3_t* b);

/** Computes cross product: out = a x b. */
void mg_vec3_cross(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b);

/** Computes length squared of vector. */
float mg_vec3_length_sqr(const mg_vec3_t* v);

/** Computes length (magnitude) of vector. */
float mg_vec3_length(const mg_vec3_t* v);

/** Normalizes vector to unit length. */
void mg_vec3_normalize(mg_vec3_t* out, const mg_vec3_t* v);

/** Computes distance between two points. */
float mg_vec3_distance(const mg_vec3_t* a, const mg_vec3_t* b);

/** Computes linear interpolation between two vectors: out = a + t * (b - a). */
void mg_vec3_lerp(mg_vec3_t* out, const mg_vec3_t* a, const mg_vec3_t* b, float t);

#ifdef __cplusplus
}
#endif

#endif /* MG_MATH_VEC3_H */

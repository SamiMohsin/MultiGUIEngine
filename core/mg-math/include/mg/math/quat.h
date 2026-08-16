/**
 * @file quat.h
 * @brief Quaternion operations for 3D rotations.
 */
#ifndef MG_MATH_QUAT_H
#define MG_MATH_QUAT_H

#include "mg/math/vec3.h"
#include "mg/math/mat4.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Quaternion structure (x, y, z, w where w is the real/scalar part).
 */
typedef struct mg_quat {
    float x;
    float y;
    float z;
    float w;
} mg_quat_t;

/** Sets quaternion components directly. */
void mg_quat_set(mg_quat_t* out, float x, float y, float z, float w);

/** Sets quaternion to identity (0, 0, 0, 1). */
void mg_quat_identity(mg_quat_t* out);

/** Copies quaternion src into out. */
void mg_quat_copy(mg_quat_t* out, const mg_quat_t* src);

/** Creates quaternion from normalized axis and angle in radians. */
void mg_quat_from_axis_angle(mg_quat_t* out, const mg_vec3_t* axis, float angle_rad);

/** Creates quaternion from Euler angles in radians (pitch/x, yaw/y, roll/z). */
void mg_quat_from_euler(mg_quat_t* out, float pitch, float yaw, float roll);

/** Multiplies two quaternions: out = a * b. */
void mg_quat_mul(mg_quat_t* out, const mg_quat_t* a, const mg_quat_t* b);

/** Computes quaternion dot product: a . b. */
float mg_quat_dot(const mg_quat_t* a, const mg_quat_t* b);

/** Computes length squared of quaternion. */
float mg_quat_length_sqr(const mg_quat_t* q);

/** Computes length (magnitude) of quaternion. */
float mg_quat_length(const mg_quat_t* q);

/** Normalizes quaternion to unit length. */
void mg_quat_normalize(mg_quat_t* out, const mg_quat_t* q);

/** Computes conjugate of quaternion: out = (-x, -y, -z, w). */
void mg_quat_conjugate(mg_quat_t* out, const mg_quat_t* q);

/** Computes spherical linear interpolation (slerp) between a and b: out = slerp(a, b, t). */
void mg_quat_slerp(mg_quat_t* out, const mg_quat_t* a, const mg_quat_t* b, float t);

/** Converts unit quaternion to a 4x4 rotation matrix. */
void mg_quat_to_mat4(mg_mat4_t* out, const mg_quat_t* q);

/** Rotates a 3D vector by a quaternion: out = q * v * q^-1. */
void mg_quat_rotate_vec3(mg_vec3_t* out, const mg_quat_t* q, const mg_vec3_t* v);

#ifdef __cplusplus
}
#endif

#endif /* MG_MATH_QUAT_H */

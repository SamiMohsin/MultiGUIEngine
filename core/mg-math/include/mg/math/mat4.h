/**
 * @file mat4.h
 * @brief 4x4 matrix operations. Row-major format matching Vulkan clip space (Y-down, depth [0, 1]).
 */
#ifndef MG_MATH_MAT4_H
#define MG_MATH_MAT4_H

#include "mg/math/vec3.h"
#include "mg/math/vec4.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 4x4 matrix in row-major layout:
 * [ m[0]  m[1]  m[2]  m[3]  ]
 * [ m[4]  m[5]  m[6]  m[7]  ]
 * [ m[8]  m[9]  m[10] m[11] ]
 * [ m[12] m[13] m[14] m[15] ]
 */
typedef struct mg_mat4 {
    float m[16];
} mg_mat4_t;

/** Sets out to the 4x4 identity matrix. */
void mg_mat4_identity(mg_mat4_t* out);

/** Sets all elements to zero. */
void mg_mat4_zero(mg_mat4_t* out);

/** Copies src matrix into out. */
void mg_mat4_copy(mg_mat4_t* out, const mg_mat4_t* src);

/** Multiplies two matrices: out = a * b. */
void mg_mat4_mul(mg_mat4_t* out, const mg_mat4_t* a, const mg_mat4_t* b);

/** Transforms a 4D vector by matrix: out = m * v. */
void mg_mat4_transform_vec4(mg_vec4_t* out, const mg_mat4_t* m, const mg_vec4_t* v);

/** Transforms a 3D point (w=1): out = (m * (v, 1)).xyz / w. */
void mg_mat4_transform_point(mg_vec3_t* out, const mg_mat4_t* m, const mg_vec3_t* v);

/** Transforms a 3D direction vector (w=0): out = (m * (v, 0)).xyz. */
void mg_mat4_transform_vector(mg_vec3_t* out, const mg_mat4_t* m, const mg_vec3_t* v);

/** Transposes matrix: out = a^T. */
void mg_mat4_transpose(mg_mat4_t* out, const mg_mat4_t* a);

/** Computes matrix inverse. Returns false if matrix is singular (non-invertible). */
bool mg_mat4_invert(mg_mat4_t* out, const mg_mat4_t* a);

/** Creates a translation matrix from a 3D vector. */
void mg_mat4_translation(mg_mat4_t* out, const mg_vec3_t* translation);

/** Creates a scaling matrix from a 3D vector. */
void mg_mat4_scaling(mg_mat4_t* out, const mg_vec3_t* scale);

/** Creates a rotation matrix from axis and angle in radians. */
void mg_mat4_rotation_axis(mg_mat4_t* out, const mg_vec3_t* axis, float angle_rad);

/** Creates a rotation matrix from Euler angles (pitch/x, yaw/y, roll/z in radians). */
void mg_mat4_rotation_euler(mg_mat4_t* out, float pitch, float yaw, float roll);

/**
 * @brief Creates a perspective projection matrix (Vulkan clip space: Y-down, depth [0, 1]).
 * @param out Output matrix.
 * @param fov_y_rad Vertical field of view in radians.
 * @param aspect Aspect ratio (width / height).
 * @param z_near Near clipping plane (> 0).
 * @param z_far Far clipping plane (> z_near).
 */
void mg_mat4_perspective(mg_mat4_t* out, float fov_y_rad, float aspect, float z_near, float z_far);

/**
 * @brief Creates an orthographic projection matrix (Vulkan clip space: Y-down, depth [0, 1]).
 * @param out Output matrix.
 * @param left Left coordinate.
 * @param right Right coordinate.
 * @param bottom Bottom coordinate.
 * @param top Top coordinate.
 * @param z_near Near depth plane.
 * @param z_far Far depth plane.
 */
void mg_mat4_ortho(mg_mat4_t* out, float left, float right, float bottom, float top, float z_near, float z_far);

/**
 * @brief Creates a look-at view matrix (right-handed).
 * @param out Output matrix.
 * @param eye Camera eye position.
 * @param target Look-at target position.
 * @param up Up direction vector.
 */
void mg_mat4_look_at(mg_mat4_t* out, const mg_vec3_t* eye, const mg_vec3_t* target, const mg_vec3_t* up);

#ifdef __cplusplus
}
#endif

#endif /* MG_MATH_MAT4_H */

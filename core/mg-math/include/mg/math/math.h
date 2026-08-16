/**
 * @file math.h
 * @brief Master header and scalar utilities for MultiGUIEngine Math Library.
 */
#ifndef MG_MATH_MATH_H
#define MG_MATH_MATH_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MG_PI
#define MG_PI 3.14159265358979323846f
#endif

#ifndef MG_PI_2
#define MG_PI_2 1.57079632679489661923f
#endif

#ifndef MG_DEG2RAD
#define MG_DEG2RAD (MG_PI / 180.0f)
#endif

#ifndef MG_RAD2DEG
#define MG_RAD2DEG (180.0f / MG_PI)
#endif

#ifndef MG_EPSILON
#define MG_EPSILON 1e-6f
#endif

/**
 * @brief Clamps a float value between min and max.
 */
static inline float mg_clamp(float val, float min, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

/**
 * @brief Returns the minimum of two float values.
 */
static inline float mg_min(float a, float b) {
    return (a < b) ? a : b;
}

/**
 * @brief Returns the maximum of two float values.
 */
static inline float mg_max(float a, float b) {
    return (a > b) ? a : b;
}

/**
 * @brief Linearly interpolates between a and b by factor t.
 */
static inline float mg_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

/**
 * @brief Converts degrees to radians.
 */
static inline float mg_deg_to_rad(float deg) {
    return deg * MG_DEG2RAD;
}

/**
 * @brief Converts radians to degrees.
 */
static inline float mg_rad_to_deg(float rad) {
    return rad * MG_RAD2DEG;
}

#ifdef __cplusplus
}
#endif

#include "mg/math/vec2.h"
#include "mg/math/vec3.h"
#include "mg/math/vec4.h"
#include "mg/math/mat4.h"
#include "mg/math/quat.h"

#endif /* MG_MATH_MATH_H */

/**
 * @file ccd.h
 * @brief MultiGUIEngine 3D Continuous Collision Detection (CCD) & Swept Intersection Tests.
 */
#ifndef MG_PHYSICS_CCD_H
#define MG_PHYSICS_CCD_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_ccd_hit {
    bool has_hit;
    float time_of_impact; /* [0.0, 1.0] */
    mg_vec3_t hit_point;
    mg_vec3_t hit_normal;
} mg_ccd_hit_t;

typedef struct mg_physics_plane {
    mg_vec3_t normal;
    float distance;
} mg_physics_plane_t;

/**
 * @brief Performs 3D continuous swept collision test between a moving sphere and a static plane.
 * @param start_pos Sphere starting position.
 * @param velocity Sphere linear movement vector over the frame timestep.
 * @param radius Sphere radius.
 * @param plane Static obstacle plane.
 * @return CCD hit structure containing time_of_impact and hit point.
 */
mg_ccd_hit_t mg_ccd_swept_sphere_plane(
    const mg_vec3_t* start_pos,
    const mg_vec3_t* velocity,
    float radius,
    const mg_physics_plane_t* plane
);

/**
 * @brief Performs 3D continuous swept collision test between two moving spheres.
 */
mg_ccd_hit_t mg_ccd_swept_sphere_sphere(
    const mg_vec3_t* pos_a,
    const mg_vec3_t* vel_a,
    float radius_a,
    const mg_vec3_t* pos_b,
    const mg_vec3_t* vel_b,
    float radius_b
);

#ifdef __cplusplus
}
#endif

#endif /* MG_PHYSICS_CCD_H */

/**
 * @file physics3d.h
 * @brief MultiGUIEngine 3D Physics Primitive Collisions and Raycasting.
 */
#ifndef MG_PHYSICS_PHYSICS3D_H
#define MG_PHYSICS_PHYSICS3D_H

#include "mg/math/vec3.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_sphere {
    mg_vec3_t center;
    float radius;
} mg_sphere_t;

typedef struct mg_aabb3d {
    mg_vec3_t min;
    mg_vec3_t max;
} mg_aabb3d_t;

typedef struct mg_ray3d {
    mg_vec3_t origin;
    mg_vec3_t direction;
} mg_ray3d_t;

typedef struct mg_raycast_hit3d {
    bool hit;
    float distance;
    mg_vec3_t point;
    mg_vec3_t normal;
} mg_raycast_hit3d_t;

/**
 * @brief Tests intersection between two 3D spheres.
 */
bool mg_physics_intersect_sphere_sphere(const mg_sphere_t* a, const mg_sphere_t* b);

/**
 * @brief Tests intersection between a 3D sphere and an AABB.
 */
bool mg_physics_intersect_sphere_aabb(const mg_sphere_t* s, const mg_aabb3d_t* b);

/**
 * @brief Tests intersection between two 3D AABBs.
 */
bool mg_physics_intersect_aabb_aabb(const mg_aabb3d_t* a, const mg_aabb3d_t* b);

/**
 * @brief Casts a 3D ray against a sphere.
 */
bool mg_physics_raycast_sphere(const mg_ray3d_t* ray, const mg_sphere_t* sphere, mg_raycast_hit3d_t* out_hit);

/**
 * @brief Casts a 3D ray against an axis-aligned bounding box.
 */
bool mg_physics_raycast_aabb(const mg_ray3d_t* ray, const mg_aabb3d_t* aabb, mg_raycast_hit3d_t* out_hit);

#ifdef __cplusplus
}
#endif

#endif /* MG_PHYSICS_PHYSICS3D_H */

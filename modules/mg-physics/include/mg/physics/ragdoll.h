/**
 * @file ragdoll.h
 * @brief MultiGUIEngine 3D Ragdoll Physics & Articulated Multi-Body Constraints.
 */
#ifndef MG_PHYSICS_RAGDOLL_H
#define MG_PHYSICS_RAGDOLL_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_MAX_RAGDOLL_BODIES 16
#define MG_MAX_RAGDOLL_JOINTS 16

typedef struct mg_ragdoll_body {
    mg_vec3_t position;
    mg_vec3_t prev_position;
    float inv_mass;
    float radius;
} mg_ragdoll_body_t;

typedef struct mg_ragdoll_joint {
    int32_t body_a;
    int32_t body_b;
    float rest_distance;
} mg_ragdoll_joint_t;

typedef struct mg_ragdoll {
    mg_ragdoll_body_t bodies[MG_MAX_RAGDOLL_BODIES];
    uint32_t body_count;
    mg_ragdoll_joint_t joints[MG_MAX_RAGDOLL_JOINTS];
    uint32_t joint_count;
    uint32_t solver_iterations;
} mg_ragdoll_t;

/**
 * @brief Initializes a ragdoll instance.
 */
void mg_ragdoll_init(mg_ragdoll_t* ragdoll);

/**
 * @brief Adds a rigid body limb to the ragdoll.
 * @return Index of created body, or -1 on error.
 */
int32_t mg_ragdoll_add_body(
    mg_ragdoll_t* ragdoll,
    const mg_vec3_t* position,
    float mass,
    float radius
);

/**
 * @brief Connects two bodies with a distance constraint joint.
 * @return Index of created joint, or -1 on error.
 */
int32_t mg_ragdoll_add_joint(
    mg_ragdoll_t* ragdoll,
    int32_t body_a,
    int32_t body_b,
    float rest_distance
);

/**
 * @brief Steps the ragdoll physics simulation using Verlet integration and constraint relaxation.
 */
void mg_ragdoll_step(
    mg_ragdoll_t* ragdoll,
    float dt,
    const mg_vec3_t* gravity
);

#ifdef __cplusplus
}
#endif

#endif /* MG_PHYSICS_RAGDOLL_H */

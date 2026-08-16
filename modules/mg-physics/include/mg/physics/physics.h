/**
 * @file physics.h
 * @brief MultiGUIEngine 2D/3D Rigid Body Physics Engine.
 */
#ifndef MG_PHYSICS_PHYSICS_H
#define MG_PHYSICS_PHYSICS_H

#include "mg/math/math.h"
#include "mg/rx/rx.h"
#include "mg/physics/physics3d.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t mg_body_handle_t;
#define MG_INVALID_BODY 0

typedef enum mg_body_type {
    MG_BODY_TYPE_STATIC = 0,
    MG_BODY_TYPE_DYNAMIC,
    MG_BODY_TYPE_KINEMATIC
} mg_body_type_t;

typedef enum mg_collider_shape {
    MG_SHAPE_BOX = 0,
    MG_SHAPE_CIRCLE
} mg_collider_shape_t;

typedef struct mg_rigid_body_desc {
    mg_body_type_t type;
    mg_vec2_t position;
    float rotation;
    mg_vec2_t linear_velocity;
    float angular_velocity;
    float mass;
    float restitution; /* Bounciness [0, 1] */
    float friction;
    mg_collider_shape_t shape;
    mg_vec2_t half_extents; /* For box */
    float radius;           /* For circle */
} mg_rigid_body_desc_t;

typedef struct mg_physics_world mg_physics_world_t;

/**
 * @brief Creates a physics simulation world.
 * @param gravity Gravity acceleration vector (e.g. {0, -9.81f}).
 * @return Pointer to physics world, or NULL on failure.
 */
mg_physics_world_t* mg_physics_world_create(mg_vec2_t gravity);

/**
 * @brief Destroys a physics simulation world.
 * @param world Pointer to physics world.
 */
void mg_physics_world_destroy(mg_physics_world_t* world);

/**
 * @brief Adds a rigid body to the physics world.
 */
mg_body_handle_t mg_physics_body_create(mg_physics_world_t* world, const mg_rigid_body_desc_t* desc);

/**
 * @brief Destroys a rigid body in the physics world.
 */
void mg_physics_body_destroy(mg_physics_world_t* world, mg_body_handle_t body);

/**
 * @brief Retrieves the current position of a body.
 */
mg_vec2_t mg_physics_body_get_position(const mg_physics_world_t* world, mg_body_handle_t body);

/**
 * @brief Retrieves the current velocity of a body.
 */
mg_vec2_t mg_physics_body_get_velocity(const mg_physics_world_t* world, mg_body_handle_t body);

/**
 * @brief Applies an impulse force to a dynamic body.
 */
void mg_physics_body_apply_impulse(mg_physics_world_t* world, mg_body_handle_t body, mg_vec2_t impulse);

/**
 * @brief Advances the physics simulation by fixed timestep dt.
 * @param world Pointer to physics world.
 * @param dt Timestep in seconds.
 */
void mg_physics_world_step(mg_physics_world_t* world, float dt);

#ifdef __cplusplus
}
#endif

#endif /* MG_PHYSICS_PHYSICS_H */

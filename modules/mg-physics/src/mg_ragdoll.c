/**
 * @file mg_ragdoll.c
 * @brief MultiGUIEngine 3D Ragdoll Physics & Articulated Multi-Body Constraints implementation.
 */
#include "mg/physics/ragdoll.h"
#include <string.h>
#include <math.h>

void mg_ragdoll_init(mg_ragdoll_t* ragdoll) {
    if (!ragdoll) return;
    memset(ragdoll, 0, sizeof(mg_ragdoll_t));
    ragdoll->solver_iterations = 4;
}

int32_t mg_ragdoll_add_body(
    mg_ragdoll_t* ragdoll,
    const mg_vec3_t* position,
    float mass,
    float radius
) {
    if (!ragdoll || ragdoll->body_count >= MG_MAX_RAGDOLL_BODIES || !position) return -1;

    int32_t idx = (int32_t)ragdoll->body_count++;
    mg_ragdoll_body_t* body = &ragdoll->bodies[idx];

    body->position = *position;
    body->prev_position = *position;
    body->inv_mass = (mass > 0.0001f) ? (1.0f / mass) : 0.0f;
    body->radius = radius;

    return idx;
}

int32_t mg_ragdoll_add_joint(
    mg_ragdoll_t* ragdoll,
    int32_t body_a,
    int32_t body_b,
    float rest_distance
) {
    if (!ragdoll || ragdoll->joint_count >= MG_MAX_RAGDOLL_JOINTS) return -1;
    if (body_a < 0 || (uint32_t)body_a >= ragdoll->body_count) return -1;
    if (body_b < 0 || (uint32_t)body_b >= ragdoll->body_count) return -1;

    int32_t idx = (int32_t)ragdoll->joint_count++;
    mg_ragdoll_joint_t* joint = &ragdoll->joints[idx];

    joint->body_a = body_a;
    joint->body_b = body_b;
    joint->rest_distance = rest_distance;

    return idx;
}

void mg_ragdoll_step(
    mg_ragdoll_t* ragdoll,
    float dt,
    const mg_vec3_t* gravity
) {
    if (!ragdoll || dt <= 0.0f) return;

    float dt_sq = dt * dt;
    mg_vec3_t grav_acc = gravity ? (mg_vec3_t){ gravity->x * dt_sq, gravity->y * dt_sq, gravity->z * dt_sq } : (mg_vec3_t){ 0.0f, 0.0f, 0.0f };

    /* 1. Verlet Integration */
    for (uint32_t i = 0; i < ragdoll->body_count; ++i) {
        mg_ragdoll_body_t* b = &ragdoll->bodies[i];
        if (b->inv_mass <= 0.0f) continue; /* Static body */

        mg_vec3_t temp = b->position;
        float vx = (b->position.x - b->prev_position.x) * 0.99f; /* 1% damping */
        float vy = (b->position.y - b->prev_position.y) * 0.99f;
        float vz = (b->position.z - b->prev_position.z) * 0.99f;

        b->position.x += vx + grav_acc.x;
        b->position.y += vy + grav_acc.y;
        b->position.z += vz + grav_acc.z;

        b->prev_position = temp;
    }

    /* 2. Position-Based Relaxation Constraints */
    uint32_t iters = ragdoll->solver_iterations ? ragdoll->solver_iterations : 4;

    for (uint32_t it = 0; it < iters; ++it) {
        for (uint32_t j = 0; j < ragdoll->joint_count; ++j) {
            const mg_ragdoll_joint_t* joint = &ragdoll->joints[j];
            mg_ragdoll_body_t* b_a = &ragdoll->bodies[joint->body_a];
            mg_ragdoll_body_t* b_b = &ragdoll->bodies[joint->body_b];

            float dx = b_b->position.x - b_a->position.x;
            float dy = b_b->position.y - b_a->position.y;
            float dz = b_b->position.z - b_a->position.z;

            float dist = sqrtf(dx * dx + dy * dy + dz * dz);
            if (dist < 0.0001f) continue;

            float diff = (dist - joint->rest_distance) / dist;
            float total_inv_mass = b_a->inv_mass + b_b->inv_mass;

            if (total_inv_mass > 0.0001f) {
                float frac_a = b_a->inv_mass / total_inv_mass;
                float frac_b = b_b->inv_mass / total_inv_mass;

                b_a->position.x += dx * diff * frac_a;
                b_a->position.y += dy * diff * frac_a;
                b_a->position.z += dz * diff * frac_a;

                b_b->position.x -= dx * diff * frac_b;
                b_b->position.y -= dy * diff * frac_b;
                b_b->position.z -= dz * diff * frac_b;
            }
        }
    }
}

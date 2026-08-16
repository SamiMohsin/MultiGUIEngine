/**
 * @file mg_physics.c
 * @brief MultiGUIEngine 2D Rigid Body Physics Engine implementation.
 */
#include "mg/physics/physics.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MG_MAX_BODIES 1024
#define MG_MAX_MANIFOLDS 2048

typedef struct {
    mg_rigid_body_desc_t desc;
    float inv_mass;
    bool is_active;
} mg_internal_body_t;

typedef struct {
    mg_body_handle_t body_a;
    mg_body_handle_t body_b;
    mg_vec2_t normal;
    float penetration;
} mg_contact_manifold_t;

struct mg_physics_world {
    mg_vec2_t gravity;
    mg_internal_body_t bodies[MG_MAX_BODIES];
    mg_contact_manifold_t manifolds[MG_MAX_MANIFOLDS];
    size_t manifold_count;
};

mg_physics_world_t* mg_physics_world_create(mg_vec2_t gravity) {
    mg_physics_world_t* w = (mg_physics_world_t*)malloc(sizeof(mg_physics_world_t));
    if (!w) return NULL;

    memset(w, 0, sizeof(mg_physics_world_t));
    w->gravity = gravity;
    return w;
}

void mg_physics_world_destroy(mg_physics_world_t* world) {
    if (world) {
        free(world);
    }
}

mg_body_handle_t mg_physics_body_create(mg_physics_world_t* world, const mg_rigid_body_desc_t* desc) {
    if (!world || !desc) return MG_INVALID_BODY;

    for (uint32_t i = 1; i < MG_MAX_BODIES; ++i) {
        if (!world->bodies[i].is_active) {
            world->bodies[i].desc = *desc;
            world->bodies[i].inv_mass = (desc->type == MG_BODY_TYPE_DYNAMIC && desc->mass > 0.0f) ?
                                        (1.0f / desc->mass) : 0.0f;
            world->bodies[i].is_active = true;
            return i;
        }
    }
    return MG_INVALID_BODY;
}

void mg_physics_body_destroy(mg_physics_world_t* world, mg_body_handle_t body) {
    if (world && body != MG_INVALID_BODY && body < MG_MAX_BODIES) {
        world->bodies[body].is_active = false;
    }
}

mg_vec2_t mg_physics_body_get_position(const mg_physics_world_t* world, mg_body_handle_t body) {
    if (!world || body == MG_INVALID_BODY || body >= MG_MAX_BODIES) {
        return (mg_vec2_t){0.0f, 0.0f};
    }
    return world->bodies[body].desc.position;
}

mg_vec2_t mg_physics_body_get_velocity(const mg_physics_world_t* world, mg_body_handle_t body) {
    if (!world || body == MG_INVALID_BODY || body >= MG_MAX_BODIES) {
        return (mg_vec2_t){0.0f, 0.0f};
    }
    return world->bodies[body].desc.linear_velocity;
}

void mg_physics_body_apply_impulse(mg_physics_world_t* world, mg_body_handle_t body, mg_vec2_t impulse) {
    if (!world || body == MG_INVALID_BODY || body >= MG_MAX_BODIES) return;
    mg_internal_body_t* b = &world->bodies[body];
    if (b->is_active && b->desc.type == MG_BODY_TYPE_DYNAMIC) {
        b->desc.linear_velocity.x += impulse.x * b->inv_mass;
        b->desc.linear_velocity.y += impulse.y * b->inv_mass;
    }
}

static bool test_circle_vs_circle(
    const mg_internal_body_t* a,
    const mg_internal_body_t* b,
    mg_vec2_t* out_normal,
    float* out_pen
) {
    float dx = b->desc.position.x - a->desc.position.x;
    float dy = b->desc.position.y - a->desc.position.y;
    float dist_sq = dx * dx + dy * dy;
    float r_sum = a->desc.radius + b->desc.radius;

    if (dist_sq > r_sum * r_sum) return false;

    float dist = sqrtf(dist_sq);
    if (dist > 1e-6f) {
        *out_normal = (mg_vec2_t){ dx / dist, dy / dist };
        *out_pen = r_sum - dist;
    } else {
        *out_normal = (mg_vec2_t){ 0.0f, 1.0f };
        *out_pen = r_sum;
    }
    return true;
}

static bool test_box_vs_box(
    const mg_internal_body_t* a,
    const mg_internal_body_t* b,
    mg_vec2_t* out_normal,
    float* out_pen
) {
    float dx = b->desc.position.x - a->desc.position.x;
    float px = (a->desc.half_extents.x + b->desc.half_extents.x) - fabsf(dx);
    if (px <= 0.0f) return false;

    float dy = b->desc.position.y - a->desc.position.y;
    float py = (a->desc.half_extents.y + b->desc.half_extents.y) - fabsf(dy);
    if (py <= 0.0f) return false;

    if (px < py) {
        float sx = (dx < 0.0f) ? -1.0f : 1.0f;
        *out_normal = (mg_vec2_t){ sx, 0.0f };
        *out_pen = px;
    } else {
        float sy = (dy < 0.0f) ? -1.0f : 1.0f;
        *out_normal = (mg_vec2_t){ 0.0f, sy };
        *out_pen = py;
    }
    return true;
}

static void detect_collisions(mg_physics_world_t* world) {
    world->manifold_count = 0;

    for (uint32_t i = 1; i < MG_MAX_BODIES; ++i) {
        if (!world->bodies[i].is_active) continue;

        for (uint32_t j = i + 1; j < MG_MAX_BODIES; ++j) {
            if (!world->bodies[j].is_active) continue;

            mg_internal_body_t* a = &world->bodies[i];
            mg_internal_body_t* b = &world->bodies[j];

            /* Ignore static vs static */
            if (a->desc.type == MG_BODY_TYPE_STATIC && b->desc.type == MG_BODY_TYPE_STATIC) {
                continue;
            }

            mg_vec2_t normal = {0};
            float pen = 0.0f;
            bool collided = false;

            if (a->desc.shape == MG_SHAPE_CIRCLE && b->desc.shape == MG_SHAPE_CIRCLE) {
                collided = test_circle_vs_circle(a, b, &normal, &pen);
            } else if (a->desc.shape == MG_SHAPE_BOX && b->desc.shape == MG_SHAPE_BOX) {
                collided = test_box_vs_box(a, b, &normal, &pen);
            }

            if (collided && world->manifold_count < MG_MAX_MANIFOLDS) {
                world->manifolds[world->manifold_count++] = (mg_contact_manifold_t){
                    .body_a = i,
                    .body_b = j,
                    .normal = normal,
                    .penetration = pen
                };
            }
        }
    }
}

static void solve_constraints(mg_physics_world_t* world) {
    for (size_t m = 0; m < world->manifold_count; ++m) {
        mg_contact_manifold_t* c = &world->manifolds[m];
        mg_internal_body_t* a = &world->bodies[c->body_a];
        mg_internal_body_t* b = &world->bodies[c->body_b];

        float total_inv_mass = a->inv_mass + b->inv_mass;
        if (total_inv_mass <= 0.0f) continue;

        /* Relative velocity: v_rel = vb - va */
        mg_vec2_t rv = {
            b->desc.linear_velocity.x - a->desc.linear_velocity.x,
            b->desc.linear_velocity.y - a->desc.linear_velocity.y
        };

        /* Velocity along normal */
        float vel_along_normal = rv.x * c->normal.x + rv.y * c->normal.y;
        if (vel_along_normal > 0.0f) continue; /* Separating */

        float restitution = fminf(a->desc.restitution, b->desc.restitution);

        /* Impulse magnitude */
        float j_mag = -(1.0f + restitution) * vel_along_normal;
        j_mag /= total_inv_mass;

        /* Apply impulse */
        mg_vec2_t impulse = { c->normal.x * j_mag, c->normal.y * j_mag };
        if (a->desc.type == MG_BODY_TYPE_DYNAMIC) {
            a->desc.linear_velocity.x -= impulse.x * a->inv_mass;
            a->desc.linear_velocity.y -= impulse.y * a->inv_mass;
        }
        if (b->desc.type == MG_BODY_TYPE_DYNAMIC) {
            b->desc.linear_velocity.x += impulse.x * b->inv_mass;
            b->desc.linear_velocity.y += impulse.y * b->inv_mass;
        }

        /* Positional correction (Baumgarte) */
        const float percent = 0.4f;
        const float slop = 0.01f;
        float correction_mag = fmaxf(c->penetration - slop, 0.0f) / total_inv_mass * percent;
        mg_vec2_t correction = { c->normal.x * correction_mag, c->normal.y * correction_mag };

        if (a->desc.type == MG_BODY_TYPE_DYNAMIC) {
            a->desc.position.x -= correction.x * a->inv_mass;
            a->desc.position.y -= correction.y * a->inv_mass;
        }
        if (b->desc.type == MG_BODY_TYPE_DYNAMIC) {
            b->desc.position.x += correction.x * b->inv_mass;
            b->desc.position.y += correction.y * b->inv_mass;
        }
    }
}

void mg_physics_world_step(mg_physics_world_t* world, float dt) {
    if (!world || dt <= 0.0f) return;

    /* Integrate forces and velocities */
    for (uint32_t i = 1; i < MG_MAX_BODIES; ++i) {
        if (!world->bodies[i].is_active || world->bodies[i].desc.type != MG_BODY_TYPE_DYNAMIC) {
            continue;
        }

        mg_internal_body_t* b = &world->bodies[i];
        b->desc.linear_velocity.x += world->gravity.x * dt;
        b->desc.linear_velocity.y += world->gravity.y * dt;

        b->desc.position.x += b->desc.linear_velocity.x * dt;
        b->desc.position.y += b->desc.linear_velocity.y * dt;
    }

    /* Collision detection & resolution */
    detect_collisions(world);
    solve_constraints(world);
}

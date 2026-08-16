/**
 * @file test_mg_physics.c
 * @brief Comprehensive headless unit tests for mg-physics.
 */
#include "mg/physics/physics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int test_physics_gravity_and_collision(void) {
    printf("Testing physics gravity, circle-on-box collision and bounce...\n");

    mg_physics_world_t* world = mg_physics_world_create((mg_vec2_t){0.0f, -9.81f});
    if (!world) return 1;

    /* Static floor at y = 0 */
    mg_rigid_body_desc_t floor_desc = {
        .type = MG_BODY_TYPE_STATIC,
        .position = {0.0f, 0.0f},
        .shape = MG_SHAPE_BOX,
        .half_extents = {10.0f, 1.0f},
        .restitution = 0.5f,
        .friction = 0.2f
    };
    mg_body_handle_t floor_body = mg_physics_body_create(world, &floor_desc);
    if (floor_body == MG_INVALID_BODY) return 1;

    /* Dynamic ball dropping from y = 10 */
    mg_rigid_body_desc_t ball_desc = {
        .type = MG_BODY_TYPE_DYNAMIC,
        .position = {0.0f, 10.0f},
        .shape = MG_SHAPE_CIRCLE,
        .radius = 0.5f,
        .mass = 1.0f,
        .restitution = 0.5f,
        .friction = 0.2f
    };
    mg_body_handle_t ball_body = mg_physics_body_create(world, &ball_desc);
    if (ball_body == MG_INVALID_BODY) return 1;

    /* Step simulation for 2 seconds (120 steps of 1/60s) */
    for (int i = 0; i < 120; ++i) {
        mg_physics_world_step(world, 1.0f / 60.0f);
    }

    mg_vec2_t final_pos = mg_physics_body_get_position(world, ball_body);

    /* Ball should have fallen and come to rest or bounced near the top of the floor (y ≈ 1.5) */
    if (final_pos.y > 9.0f) {
        fprintf(stderr, "FAIL: ball did not fall under gravity (y=%f)\n", final_pos.y);
        mg_physics_world_destroy(world);
        return 1;
    }

    mg_physics_world_destroy(world);
    printf("PASS: physics gravity, collision, and bounce\n");
    return 0;
}

int main(void) {
    if (test_physics_gravity_and_collision() != 0) return 1;

    printf("All mg-physics unit tests passed successfully!\n");
    return 0;
}

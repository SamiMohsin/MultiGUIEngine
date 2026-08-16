/**
 * @file main.c
 * @brief Example 03: ECS Scene and Staged System Scheduler.
 */
#include "mg/scene/scene.h"
#include <stdio.h>
#include <stdlib.h>

#define COMP_POSITION 0
#define COMP_VELOCITY 1

typedef struct {
    float x, y, z;
} position_comp_t;

typedef struct {
    float vx, vy, vz;
} velocity_comp_t;

typedef struct {
    float dt;
} move_ctx_t;

static void update_position_entity(mg_world_t* world, mg_entity_t entity, void* ctx) {
    move_ctx_t* mctx = (move_ctx_t*)ctx;
    position_comp_t* pos = (position_comp_t*)mg_component_get(world, entity, COMP_POSITION);
    velocity_comp_t* vel = (velocity_comp_t*)mg_component_get(world, entity, COMP_VELOCITY);
    if (pos && vel) {
        pos->x += vel->vx * mctx->dt;
        pos->y += vel->vy * mctx->dt;
        pos->z += vel->vz * mctx->dt;
    }
}

static void movement_system(mg_world_t* world, float dt, void* ctx) {
    (void)ctx;
    mg_component_mask_t mask = (1ULL << COMP_POSITION) | (1ULL << COMP_VELOCITY);
    move_ctx_t mctx = { dt };
    mg_world_query(world, mask, update_position_entity, &mctx);
}

int main(void) {
    printf("Running Example 03: ECS Scene...\n");

    mg_world_t* world = mg_world_create();
    if (!world) {
        fprintf(stderr, "FAIL: mg_world_create failed\n");
        return 1;
    }

    /* Register components */
    mg_component_register(world, COMP_POSITION, sizeof(position_comp_t));
    mg_component_register(world, COMP_VELOCITY, sizeof(velocity_comp_t));

    /* Register systems */
    mg_system_register(world, movement_system, NULL, MG_STAGE_UPDATE);

    /* Spawn entities */
    mg_entity_t e1 = mg_entity_create(world);
    position_comp_t* p1 = (position_comp_t*)mg_component_add(world, e1, COMP_POSITION);
    velocity_comp_t* v1 = (velocity_comp_t*)mg_component_add(world, e1, COMP_VELOCITY);
    p1->x = 0.0f; p1->y = 0.0f; p1->z = 0.0f;
    v1->vx = 10.0f; v1->vy = 20.0f; v1->vz = 0.0f;

    mg_entity_t e2 = mg_entity_create(world);
    position_comp_t* p2 = (position_comp_t*)mg_component_add(world, e2, COMP_POSITION);
    p2->x = 100.0f; p2->y = 100.0f; p2->z = 0.0f;

    /* Tick world 10 times with dt = 0.1s (total 1.0 second) */
    for (int step = 0; step < 10; ++step) {
        mg_world_tick(world, 0.1f);
    }

    position_comp_t* p1_final = (position_comp_t*)mg_component_get(world, e1, COMP_POSITION);
    position_comp_t* p2_final = (position_comp_t*)mg_component_get(world, e2, COMP_POSITION);

    if (p1_final->x < 9.9f || p1_final->x > 10.1f || p1_final->y < 19.9f || p1_final->y > 20.1f) {
        fprintf(stderr, "FAIL: e1 position incorrect (%f, %f)\n", p1_final->x, p1_final->y);
        mg_world_destroy(world);
        return 1;
    }

    if (p2_final->x != 100.0f || p2_final->y != 100.0f) {
        fprintf(stderr, "FAIL: e2 position should remain static (%f, %f)\n", p2_final->x, p2_final->y);
        mg_world_destroy(world);
        return 1;
    }

    mg_world_destroy(world);
    printf("PASS: Example 03: ECS Scene simulation completed successfully\n");
    return 0;
}

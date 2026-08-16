/**
 * @file test_mg_scene.c
 * @brief Comprehensive headless unit tests for mg-scene (ECS).
 */
#include "mg/scene/scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMP_POS 0
#define COMP_VEL 1
#define COMP_HEALTH 2

typedef struct {
    float x, y, z;
} pos_t;

typedef struct {
    float vx, vy, vz;
} vel_t;

typedef struct {
    int hp;
} health_t;

static int g_system_exec_order[6] = {0};
static int g_system_exec_count = 0;

static void track_input_system(mg_world_t* w, float dt, void* ctx) {
    (void)w; (void)dt; (void)ctx;
    g_system_exec_order[g_system_exec_count++] = MG_STAGE_INPUT;
}

static void track_fixed_update_system(mg_world_t* w, float dt, void* ctx) {
    (void)w; (void)dt; (void)ctx;
    g_system_exec_order[g_system_exec_count++] = MG_STAGE_FIXED_UPDATE;
}

static void track_update_system(mg_world_t* w, float dt, void* ctx) {
    (void)w; (void)dt; (void)ctx;
    g_system_exec_order[g_system_exec_count++] = MG_STAGE_UPDATE;
}

static void track_late_update_system(mg_world_t* w, float dt, void* ctx) {
    (void)w; (void)dt; (void)ctx;
    g_system_exec_order[g_system_exec_count++] = MG_STAGE_LATE_UPDATE;
}

static void track_render_extract_system(mg_world_t* w, float dt, void* ctx) {
    (void)w; (void)dt; (void)ctx;
    g_system_exec_order[g_system_exec_count++] = MG_STAGE_RENDER_EXTRACT;
}

static void track_render_system(mg_world_t* w, float dt, void* ctx) {
    (void)w; (void)dt; (void)ctx;
    g_system_exec_order[g_system_exec_count++] = MG_STAGE_RENDER;
}

static int test_ecs_lifecycle_and_components(void) {
    printf("Testing ECS entities and components...\n");
    mg_world_t* world = mg_world_create();
    if (!world) return 1;

    mg_component_register(world, COMP_POS, sizeof(pos_t));
    mg_component_register(world, COMP_VEL, sizeof(vel_t));
    mg_component_register(world, COMP_HEALTH, sizeof(health_t));

    mg_entity_t e1 = mg_entity_create(world);
    if (!mg_entity_is_alive(world, e1)) {
        fprintf(stderr, "FAIL: e1 is not alive\n");
        return 1;
    }

    pos_t* p = (pos_t*)mg_component_add(world, e1, COMP_POS);
    p->x = 10.0f; p->y = 20.0f; p->z = 30.0f;

    if (!mg_component_has(world, e1, COMP_POS)) {
        fprintf(stderr, "FAIL: e1 missing COMP_POS\n");
        return 1;
    }
    if (mg_component_has(world, e1, COMP_VEL)) {
        fprintf(stderr, "FAIL: e1 should not have COMP_VEL\n");
        return 1;
    }

    pos_t* fetched_p = (pos_t*)mg_component_get(world, e1, COMP_POS);
    if (!fetched_p || fetched_p->x != 10.0f || fetched_p->y != 20.0f || fetched_p->z != 30.0f) {
        fprintf(stderr, "FAIL: fetched component data mismatch\n");
        return 1;
    }

    /* Destroy entity and verify stale handle */
    mg_entity_destroy(world, e1);
    if (mg_entity_is_alive(world, e1)) {
        fprintf(stderr, "FAIL: e1 should be dead after destruction\n");
        return 1;
    }

    /* Next entity should reuse slot with new generation */
    mg_entity_t e2 = mg_entity_create(world);
    if (e2 == e1) {
        fprintf(stderr, "FAIL: e2 should have different generation than e1\n");
        return 1;
    }
    if (!mg_entity_is_alive(world, e2)) {
        fprintf(stderr, "FAIL: e2 is not alive\n");
        return 1;
    }

    mg_world_destroy(world);
    printf("PASS: ECS entities and components\n");
    return 0;
}

static int test_ecs_system_stages(void) {
    printf("Testing ECS system scheduler fixed stage order...\n");
    mg_world_t* world = mg_world_create();
    if (!world) return 1;

    g_system_exec_count = 0;
    memset(g_system_exec_order, 0, sizeof(g_system_exec_order));

    /* Register out of order */
    mg_system_register(world, track_render_system, NULL, MG_STAGE_RENDER);
    mg_system_register(world, track_update_system, NULL, MG_STAGE_UPDATE);
    mg_system_register(world, track_input_system, NULL, MG_STAGE_INPUT);
    mg_system_register(world, track_late_update_system, NULL, MG_STAGE_LATE_UPDATE);
    mg_system_register(world, track_fixed_update_system, NULL, MG_STAGE_FIXED_UPDATE);
    mg_system_register(world, track_render_extract_system, NULL, MG_STAGE_RENDER_EXTRACT);

    /* Tick world */
    mg_world_tick(world, 0.016f);

    if (g_system_exec_count != 6) {
        fprintf(stderr, "FAIL: expected 6 executed systems, got %d\n", g_system_exec_count);
        mg_world_destroy(world);
        return 1;
    }

    for (int i = 0; i < 6; ++i) {
        if (g_system_exec_order[i] != i) {
            fprintf(stderr, "FAIL: stage execution order wrong at index %d (got %d expected %d)\n",
                    i, g_system_exec_order[i], i);
            mg_world_destroy(world);
            return 1;
        }
    }

    mg_world_destroy(world);
    printf("PASS: ECS system scheduler fixed stage order\n");
    return 0;
}

typedef struct {
    int matching_count;
} query_test_ctx_t;

static void count_matching_entities(mg_world_t* world, mg_entity_t entity, void* ctx) {
    (void)world; (void)entity;
    query_test_ctx_t* qctx = (query_test_ctx_t*)ctx;
    qctx->matching_count++;
}

static int test_ecs_query(void) {
    printf("Testing ECS bitmask queries...\n");
    mg_world_t* world = mg_world_create();
    if (!world) return 1;

    mg_component_register(world, COMP_POS, sizeof(pos_t));
    mg_component_register(world, COMP_VEL, sizeof(vel_t));
    mg_component_register(world, COMP_HEALTH, sizeof(health_t));

    /* e1: Pos + Vel */
    mg_entity_t e1 = mg_entity_create(world);
    mg_component_add(world, e1, COMP_POS);
    mg_component_add(world, e1, COMP_VEL);

    /* e2: Pos + Health */
    mg_entity_t e2 = mg_entity_create(world);
    mg_component_add(world, e2, COMP_POS);
    mg_component_add(world, e2, COMP_HEALTH);

    /* e3: Pos + Vel + Health */
    mg_entity_t e3 = mg_entity_create(world);
    mg_component_add(world, e3, COMP_POS);
    mg_component_add(world, e3, COMP_VEL);
    mg_component_add(world, e3, COMP_HEALTH);

    /* Query: Pos + Vel -> matches e1, e3 (count = 2) */
    query_test_ctx_t qctx = { 0 };
    mg_component_mask_t mask_pos_vel = (1ULL << COMP_POS) | (1ULL << COMP_VEL);
    mg_world_query(world, mask_pos_vel, count_matching_entities, &qctx);

    if (qctx.matching_count != 2) {
        fprintf(stderr, "FAIL: query pos+vel count %d != 2\n", qctx.matching_count);
        mg_world_destroy(world);
        return 1;
    }

    mg_world_destroy(world);
    printf("PASS: ECS bitmask queries\n");
    return 0;
}

int main(void) {
    if (test_ecs_lifecycle_and_components() != 0) return 1;
    if (test_ecs_system_stages() != 0) return 1;
    if (test_ecs_query() != 0) return 1;

    printf("All mg-scene unit tests passed successfully!\n");
    return 0;
}

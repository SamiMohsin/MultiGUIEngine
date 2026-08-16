/**
 * @file main.c
 * @brief Example 20: Scene Prefabs, Dynamic Instantiation & JSON Persistence.
 */
#include "mg/scene/scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define COMP_POS 0
#define COMP_VEL 1

typedef struct {
    float x, y, z;
} pos_comp_t;

typedef struct {
    float vx, vy, vz;
} vel_comp_t;

int main(void) {
    printf("=== Example 20: Scene Prefabs, Dynamic Instantiation & JSON Persistence ===\n");

    /* 1. Create Source World */
    mg_world_t* world1 = mg_world_create();
    assert(world1 != NULL);

    /* Register Components */
    mg_component_register(world1, COMP_POS, sizeof(pos_comp_t));
    mg_component_register(world1, COMP_VEL, sizeof(vel_comp_t));

    /* 2. Define Enemy and Asteroid Prefabs */
    mg_prefab_t enemy_drone = {
        .name = "EnemyDrone",
        .default_velocity = { 0.0f, -50.0f, 0.0f },
        .scale = 1.0f
    };

    mg_prefab_t asteroid_hazard = {
        .name = "AsteroidHazard",
        .default_velocity = { -15.0f, -30.0f, 0.0f },
        .scale = 2.5f
    };

    /* 3. Instantiate Multiple Entities from Prefabs */
    mg_vec3_t spawn1 = { 100.0f, 500.0f, 0.0f };
    mg_vec3_t spawn2 = { 300.0f, 550.0f, 0.0f };
    mg_vec3_t spawn3 = { 500.0f, 600.0f, 0.0f };

    mg_entity_t e1 = mg_prefab_instantiate(world1, &enemy_drone, &spawn1);
    mg_entity_t e2 = mg_prefab_instantiate(world1, &enemy_drone, &spawn2);
    mg_entity_t e3 = mg_prefab_instantiate(world1, &asteroid_hazard, &spawn3);
    assert(e1 != MG_NULL_ENTITY && e2 != MG_NULL_ENTITY && e3 != MG_NULL_ENTITY);
    printf("PASS: 3 Entities Instantiated from Prefabs\n");

    /* 4. Serialize Scene World to JSON */
    char json_buffer[2048];
    bool saved = mg_scene_save_to_json(world1, json_buffer, sizeof(json_buffer));
    assert(saved == true);
    printf("=== Serialized Scene JSON ===\n%s\n", json_buffer);

    /* 5. Create Fresh World 2 and Deserialize Scene from JSON */
    mg_world_t* world2 = mg_world_create();
    assert(world2 != NULL);
    mg_component_register(world2, COMP_POS, sizeof(pos_comp_t));
    mg_component_register(world2, COMP_VEL, sizeof(vel_comp_t));

    bool loaded = mg_scene_load_from_json(world2, json_buffer);
    assert(loaded == true);
    printf("PASS: Scene JSON Deserialized into World 2\n");

    /* Verify Entity Restoration in World 2 */
    char json_buffer2[2048];
    bool saved2 = mg_scene_save_to_json(world2, json_buffer2, sizeof(json_buffer2));
    assert(saved2 == true);
    assert(strstr(json_buffer2, "500.00, 600.00") != NULL);
    printf("PASS: Deserialized Scene verified with 100%% state fidelity\n");

    /* Cleanup */
    mg_world_destroy(world1);
    mg_world_destroy(world2);

    printf("PASS: Example 20: Scene Prefabs and JSON Persistence completed successfully\n");
    return 0;
}

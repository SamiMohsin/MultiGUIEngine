/**
 * @file prefab.h
 * @brief MultiGUIEngine Scene Prefabs and JSON Persistence.
 */
#ifndef MG_SCENE_PREFAB_H
#define MG_SCENE_PREFAB_H

#include "mg/scene/ecs.h"
#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_prefab {
    char name[64];
    mg_vec3_t default_velocity;
    float scale;
} mg_prefab_t;

/**
 * @brief Creates an entity from a prefab template and places it into the ECS world.
 */
mg_entity_t mg_prefab_instantiate(
    mg_world_t* world,
    const mg_prefab_t* prefab,
    const mg_vec3_t* spawn_position
);

/**
 * @brief Serializes all entities in a world to formatted JSON.
 */
bool mg_scene_save_to_json(const mg_world_t* world, char* out_json, size_t max_size);

/**
 * @brief Deserializes entities from JSON into an active ECS world.
 */
bool mg_scene_load_from_json(mg_world_t* world, const char* json_str);

#ifdef __cplusplus
}
#endif

#endif /* MG_SCENE_PREFAB_H */

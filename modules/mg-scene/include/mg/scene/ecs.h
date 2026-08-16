/**
 * @file ecs.h
 * @brief Archetype-based Entity Component System (ECS) and staged system scheduler.
 */
#ifndef MG_SCENE_ECS_H
#define MG_SCENE_ECS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Core ECS types */
typedef uint64_t mg_entity_t;
typedef uint32_t mg_component_id_t;
typedef uint64_t mg_component_mask_t;

#define MG_NULL_ENTITY 0
#define MG_MAX_COMPONENTS 64

/**
 * @brief Fixed frame execution stages in deterministic order.
 */
typedef enum mg_stage {
    MG_STAGE_INPUT = 0,          /**< Input event draining and input processing. */
    MG_STAGE_FIXED_UPDATE,      /**< Fixed-timestep physics simulation (0..N per frame). */
    MG_STAGE_UPDATE,            /**< Main game and gameplay logic updates. */
    MG_STAGE_LATE_UPDATE,       /**< Post-update logic (camera positioning, transforms). */
    MG_STAGE_RENDER_EXTRACT,    /**< Extraction of renderable snapshots to render queue. */
    MG_STAGE_RENDER,            /**< Rendering command submission to RHI. */
    MG_STAGE_COUNT
} mg_stage_t;

/* Opaque ECS World */
typedef struct mg_world mg_world_t;

/** System update function callback. */
typedef void (*mg_system_fn)(mg_world_t* world, float dt, void* ctx);

/** Entity query callback. */
typedef void (*mg_query_fn)(mg_world_t* world, mg_entity_t entity, void* ctx);

/**
 * @brief Creates a new ECS world instance.
 * @return Pointer to world, or NULL on failure.
 */
mg_world_t* mg_world_create(void);

/**
 * @brief Destroys an ECS world and all associated entities, components, and systems.
 * @param world Pointer to world. Safe to pass NULL.
 */
void mg_world_destroy(mg_world_t* world);

/**
 * @brief Creates a new alive entity with a unique ID.
 * @param world Pointer to world.
 * @return Unique entity ID.
 */
mg_entity_t mg_entity_create(mg_world_t* world);

/**
 * @brief Destroys an entity and removes all its components.
 * @param world Pointer to world.
 * @param entity Entity ID to destroy.
 */
void mg_entity_destroy(mg_world_t* world, mg_entity_t entity);

/**
 * @brief Checks if an entity exists and is alive.
 */
bool mg_entity_is_alive(const mg_world_t* world, mg_entity_t entity);

/**
 * @brief Registers a component type with the world.
 * @param world Pointer to world.
 * @param id Unique component type identifier (0 <= id < 64).
 * @param size Byte size of component struct.
 */
void mg_component_register(mg_world_t* world, mg_component_id_t id, size_t size);

/**
 * @brief Adds a component to an entity.
 * @param world Pointer to world.
 * @param entity Target entity ID.
 * @param id Component type ID.
 * @return Pointer to allocated component data, or NULL on failure.
 */
void* mg_component_add(mg_world_t* world, mg_entity_t entity, mg_component_id_t id);

/**
 * @brief Retrieves a component pointer for an entity.
 * @param world Pointer to world.
 * @param entity Target entity ID.
 * @param id Component type ID.
 * @return Pointer to component data, or NULL if entity lacks component.
 */
void* mg_component_get(const mg_world_t* world, mg_entity_t entity, mg_component_id_t id);

/**
 * @brief Checks if an entity possesses a component.
 */
bool mg_component_has(const mg_world_t* world, mg_entity_t entity, mg_component_id_t id);

/**
 * @brief Removes a component from an entity.
 */
void mg_component_remove(mg_world_t* world, mg_entity_t entity, mg_component_id_t id);

/**
 * @brief Registers a system to be executed during a specific frame stage.
 * @param world Pointer to world.
 * @param fn System function.
 * @param ctx Context pointer passed to fn.
 * @param stage Frame stage in which system executes.
 */
void mg_system_register(mg_world_t* world, mg_system_fn fn, void* ctx, mg_stage_t stage);

/**
 * @brief Executes all systems in a given stage with delta time dt.
 * @param world Pointer to world.
 * @param stage Target stage.
 * @param dt Delta time in seconds.
 */
void mg_world_tick_stage(mg_world_t* world, mg_stage_t stage, float dt);

/**
 * @brief Executes a full frame tick through all stages in fixed order.
 * @param world Pointer to world.
 * @param dt Delta time in seconds.
 */
void mg_world_tick(mg_world_t* world, float dt);

/**
 * @brief Iterates over all entities matching the required component mask.
 * @param world Pointer to world.
 * @param required_mask Bitmask of required component IDs (1ULL << id).
 * @param callback Function invoked for each matching entity.
 * @param ctx Context pointer passed to callback.
 */
void mg_world_query(
    mg_world_t* world,
    mg_component_mask_t required_mask,
    mg_query_fn callback,
    void* ctx
);

#ifdef __cplusplus
}
#endif

#endif /* MG_SCENE_ECS_H */

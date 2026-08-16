/**
 * @file mg_ecs.c
 * @brief High-performance Entity Component System implementation.
 */
#include "mg/scene/ecs.h"
#include "mg/alloc/alloc.h"
#include <stdlib.h>
#include <string.h>

#define MG_ECS_MAX_ENTITIES 4096
#define MG_ECS_MAX_SYSTEMS_PER_STAGE 32

typedef struct {
    mg_system_fn fn;
    void* ctx;
} mg_system_entry_t;

typedef struct {
    size_t size;
    uint8_t* data; /* Indexed by entity index: data[entity_idx * size] */
    bool is_registered;
} mg_component_type_info_t;

struct mg_world {
    uint32_t next_entity_index;
    uint32_t active_entity_count;

    bool entity_alive[MG_ECS_MAX_ENTITIES];
    uint32_t entity_generation[MG_ECS_MAX_ENTITIES];
    mg_component_mask_t entity_mask[MG_ECS_MAX_ENTITIES];

    mg_component_type_info_t components[MG_MAX_COMPONENTS];

    mg_system_entry_t systems[MG_STAGE_COUNT][MG_ECS_MAX_SYSTEMS_PER_STAGE];
    size_t system_count[MG_STAGE_COUNT];
};

static inline uint32_t entity_to_index(mg_entity_t e) {
    return (uint32_t)(e & 0xFFFFFFFF);
}

static inline uint32_t entity_to_gen(mg_entity_t e) {
    return (uint32_t)((e >> 32) & 0xFFFFFFFF);
}

static inline mg_entity_t make_entity(uint32_t index, uint32_t generation) {
    return ((uint64_t)generation << 32) | (uint64_t)index;
}

mg_world_t* mg_world_create(void) {
    mg_world_t* w = (mg_world_t*)malloc(sizeof(mg_world_t));
    if (!w) return NULL;

    memset(w, 0, sizeof(mg_world_t));
    w->next_entity_index = 1; /* 0 is reserved for NULL_ENTITY */

    return w;
}

void mg_world_destroy(mg_world_t* world) {
    if (!world) return;

    for (size_t i = 0; i < MG_MAX_COMPONENTS; ++i) {
        if (world->components[i].is_registered && world->components[i].data) {
            free(world->components[i].data);
        }
    }

    free(world);
}

mg_entity_t mg_entity_create(mg_world_t* world) {
    if (!world) return MG_NULL_ENTITY;

    /* Search for available reusable slot or allocate next */
    uint32_t idx = 0;
    uint32_t limit = world->next_entity_index < MG_ECS_MAX_ENTITIES ? world->next_entity_index : MG_ECS_MAX_ENTITIES;
    for (uint32_t i = 1; i < limit; ++i) {
        if (!world->entity_alive[i]) {
            idx = i;
            break;
        }
    }

    if (idx == 0) {
        if (world->next_entity_index >= MG_ECS_MAX_ENTITIES) {
            return MG_NULL_ENTITY; /* Maximum entities reached */
        }
        idx = world->next_entity_index++;
    }

    world->entity_alive[idx] = true;
    world->entity_generation[idx]++;
    world->entity_mask[idx] = 0;
    world->active_entity_count++;

    return make_entity(idx, world->entity_generation[idx]);
}

void mg_entity_destroy(mg_world_t* world, mg_entity_t entity) {
    if (!world || !mg_entity_is_alive(world, entity)) {
        return;
    }

    uint32_t idx = entity_to_index(entity);
    world->entity_alive[idx] = false;
    world->entity_mask[idx] = 0;
    if (world->active_entity_count > 0) {
        world->active_entity_count--;
    }
}

bool mg_entity_is_alive(const mg_world_t* world, mg_entity_t entity) {
    if (!world || entity == MG_NULL_ENTITY) return false;
    uint32_t idx = entity_to_index(entity);
    uint32_t gen = entity_to_gen(entity);

    if (idx >= MG_ECS_MAX_ENTITIES || !world->entity_alive[idx]) {
        return false;
    }

    return world->entity_generation[idx] == gen;
}

void mg_component_register(mg_world_t* world, mg_component_id_t id, size_t size) {
    if (!world || id >= MG_MAX_COMPONENTS || size == 0) {
        return;
    }

    if (world->components[id].is_registered && world->components[id].data) {
        free(world->components[id].data);
    }

    world->components[id].size = size;
    world->components[id].data = (uint8_t*)malloc(size * MG_ECS_MAX_ENTITIES);
    if (world->components[id].data) {
        memset(world->components[id].data, 0, size * MG_ECS_MAX_ENTITIES);
        world->components[id].is_registered = true;
    }
}

void* mg_component_add(mg_world_t* world, mg_entity_t entity, mg_component_id_t id) {
    if (!world || !mg_entity_is_alive(world, entity) || id >= MG_MAX_COMPONENTS) {
        return NULL;
    }

    if (!world->components[id].is_registered || !world->components[id].data) {
        return NULL;
    }

    uint32_t idx = entity_to_index(entity);
    world->entity_mask[idx] |= (1ULL << id);

    void* comp_ptr = world->components[id].data + (idx * world->components[id].size);
    memset(comp_ptr, 0, world->components[id].size);
    return comp_ptr;
}

void* mg_component_get(const mg_world_t* world, mg_entity_t entity, mg_component_id_t id) {
    if (!world || !mg_entity_is_alive(world, entity) || id >= MG_MAX_COMPONENTS) {
        return NULL;
    }

    uint32_t idx = entity_to_index(entity);
    if ((world->entity_mask[idx] & (1ULL << id)) == 0) {
        return NULL;
    }

    return world->components[id].data + (idx * world->components[id].size);
}

bool mg_component_has(const mg_world_t* world, mg_entity_t entity, mg_component_id_t id) {
    if (!world || !mg_entity_is_alive(world, entity) || id >= MG_MAX_COMPONENTS) {
        return false;
    }
    uint32_t idx = entity_to_index(entity);
    return (world->entity_mask[idx] & (1ULL << id)) != 0;
}

void mg_component_remove(mg_world_t* world, mg_entity_t entity, mg_component_id_t id) {
    if (!world || !mg_entity_is_alive(world, entity) || id >= MG_MAX_COMPONENTS) {
        return;
    }
    uint32_t idx = entity_to_index(entity);
    world->entity_mask[idx] &= ~(1ULL << id);
}

void mg_system_register(mg_world_t* world, mg_system_fn fn, void* ctx, mg_stage_t stage) {
    if (!world || !fn || stage >= MG_STAGE_COUNT) {
        return;
    }

    size_t count = world->system_count[stage];
    if (count < MG_ECS_MAX_SYSTEMS_PER_STAGE) {
        world->systems[stage][count].fn = fn;
        world->systems[stage][count].ctx = ctx;
        world->system_count[stage]++;
    }
}

void mg_world_tick_stage(mg_world_t* world, mg_stage_t stage, float dt) {
    if (!world || stage >= MG_STAGE_COUNT) return;

    size_t count = world->system_count[stage];
    for (size_t i = 0; i < count; ++i) {
        if (world->systems[stage][i].fn) {
            world->systems[stage][i].fn(world, dt, world->systems[stage][i].ctx);
        }
    }
}

void mg_world_tick(mg_world_t* world, float dt) {
    if (!world) return;

    for (int s = 0; s < (int)MG_STAGE_COUNT; ++s) {
        mg_world_tick_stage(world, (mg_stage_t)s, dt);
    }
}

void mg_world_query(
    mg_world_t* world,
    mg_component_mask_t required_mask,
    mg_query_fn callback,
    void* ctx
) {
    if (!world || !callback) return;

    for (uint32_t i = 1; i < world->next_entity_index; ++i) {
        if (world->entity_alive[i]) {
            if ((world->entity_mask[i] & required_mask) == required_mask) {
                mg_entity_t entity = make_entity(i, world->entity_generation[i]);
                callback(world, entity, ctx);
            }
        }
    }
}

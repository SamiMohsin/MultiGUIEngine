/**
 * @file mg_prefab.c
 * @brief MultiGUIEngine Scene Prefabs and JSON Persistence implementation.
 */
#include "mg/scene/prefab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMP_POS 0
#define COMP_VEL 1

typedef struct {
    float x, y, z;
} pos_comp_t;

typedef struct {
    float vx, vy, vz;
} vel_comp_t;

mg_entity_t mg_prefab_instantiate(
    mg_world_t* world,
    const mg_prefab_t* prefab,
    const mg_vec3_t* spawn_position
) {
    if (!world || !prefab) return MG_NULL_ENTITY;

    mg_entity_t entity = mg_entity_create(world);
    if (entity == MG_NULL_ENTITY) return MG_NULL_ENTITY;

    pos_comp_t* pos = (pos_comp_t*)mg_component_add(world, entity, COMP_POS);
    if (pos) {
        pos->x = spawn_position ? spawn_position->x : 0.0f;
        pos->y = spawn_position ? spawn_position->y : 0.0f;
        pos->z = spawn_position ? spawn_position->z : 0.0f;
    }

    vel_comp_t* vel = (vel_comp_t*)mg_component_add(world, entity, COMP_VEL);
    if (vel) {
        vel->vx = prefab->default_velocity.x;
        vel->vy = prefab->default_velocity.y;
        vel->vz = prefab->default_velocity.z;
    }

    return entity;
}

typedef struct {
    char* buffer;
    size_t max_size;
    size_t offset;
    uint32_t count;
} json_serialize_ctx_t;

static void serialize_entity_cb(mg_world_t* world, mg_entity_t entity, void* ctx) {
    json_serialize_ctx_t* jctx = (json_serialize_ctx_t*)ctx;
    pos_comp_t* pos = (pos_comp_t*)mg_component_get(world, entity, COMP_POS);
    vel_comp_t* vel = (vel_comp_t*)mg_component_get(world, entity, COMP_VEL);

    if (pos && vel && jctx->offset < jctx->max_size) {
        int written = snprintf(
            jctx->buffer + jctx->offset,
            jctx->max_size - jctx->offset,
            "%s{\"id\": %llu, \"pos\": [%.2f, %.2f, %.2f], \"vel\": [%.2f, %.2f, %.2f]}",
            (jctx->count > 0) ? ",\n    " : "    ",
            (unsigned long long)entity,
            pos->x, pos->y, pos->z,
            vel->vx, vel->vy, vel->vz
        );
        if (written > 0) {
            jctx->offset += (size_t)written;
            jctx->count++;
        }
    }
}

bool mg_scene_save_to_json(const mg_world_t* world, char* out_json, size_t max_size) {
    if (!world || !out_json || max_size == 0) return false;

    json_serialize_ctx_t jctx = {
        .buffer = out_json,
        .max_size = max_size,
        .offset = 0,
        .count = 0
    };

    int prefix = snprintf(jctx.buffer, max_size, "{\n  \"version\": 1,\n  \"entities\": [\n");
    if (prefix <= 0) return false;
    jctx.offset = (size_t)prefix;

    mg_component_mask_t mask = (1ULL << COMP_POS) | (1ULL << COMP_VEL);
    mg_world_query((mg_world_t*)world, mask, serialize_entity_cb, &jctx);

    if (jctx.offset < max_size) {
        snprintf(jctx.buffer + jctx.offset, max_size - jctx.offset, "\n  ]\n}\n");
        return true;
    }
    return false;
}

bool mg_scene_load_from_json(mg_world_t* world, const char* json_str) {
    if (!world || !json_str) return false;

    /* Simple scanner for entities array */
    const char* ptr = json_str;
    while ((ptr = strstr(ptr, "\"pos\": [")) != NULL) {
        ptr += 8;
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float vx = 0.0f, vy = 0.0f, vz = 0.0f;

        if (sscanf(ptr, "%f, %f, %f", &x, &y, &z) == 3) {
            const char* vel_ptr = strstr(ptr, "\"vel\": [");
            if (vel_ptr) {
                vel_ptr += 8;
                sscanf(vel_ptr, "%f, %f, %f", &vx, &vy, &vz);
            }

            mg_entity_t ent = mg_entity_create(world);
            pos_comp_t* p = (pos_comp_t*)mg_component_add(world, ent, COMP_POS);
            if (p) { p->x = x; p->y = y; p->z = z; }
            vel_comp_t* v = (vel_comp_t*)mg_component_add(world, ent, COMP_VEL);
            if (v) { v->vx = vx; v->vy = vy; v->vz = vz; }
        }
    }
    return true;
}

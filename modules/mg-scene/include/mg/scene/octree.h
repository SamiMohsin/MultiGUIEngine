/**
 * @file octree.h
 * @brief MultiGUIEngine 3D Spatial Octree Partitioning & Culling Engine.
 */
#ifndef MG_SCENE_OCTREE_H
#define MG_SCENE_OCTREE_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_OCTREE_MAX_ITEMS_PER_NODE 8

typedef struct mg_octree_item {
    uint32_t entity_id;
    mg_vec3_t min;
    mg_vec3_t max;
} mg_octree_item_t;

typedef struct mg_octree_node {
    mg_vec3_t min;
    mg_vec3_t max;
    mg_octree_item_t items[MG_OCTREE_MAX_ITEMS_PER_NODE];
    uint32_t item_count;
    struct mg_octree_node* children[8];
    bool is_leaf;
} mg_octree_node_t;

typedef struct mg_octree {
    mg_octree_node_t* root;
    uint32_t max_depth;
    uint32_t total_elements;
} mg_octree_t;

/**
 * @brief Initializes a 3D spatial octree with given world bounds and maximum depth.
 */
void mg_octree_init(
    mg_octree_t* tree,
    const mg_vec3_t* world_min,
    const mg_vec3_t* world_max,
    uint32_t max_depth
);

/**
 * @brief Inserts an entity with an AABB bounding volume into the octree.
 * @return True on success, false if out of bounds or allocation failure.
 */
bool mg_octree_insert(
    mg_octree_t* tree,
    uint32_t entity_id,
    const mg_vec3_t* obj_min,
    const mg_vec3_t* obj_max
);

/**
 * @brief Queries all entities overlapping with a target AABB bounding volume.
 * @return Total number of matching entities found (capped at max_results).
 */
size_t mg_octree_query_range(
    const mg_octree_t* tree,
    const mg_vec3_t* query_min,
    const mg_vec3_t* query_max,
    uint32_t* out_entity_ids,
    size_t max_results
);

/**
 * @brief Frees all allocated octree nodes.
 */
void mg_octree_free(mg_octree_t* tree);

#ifdef __cplusplus
}
#endif

#endif /* MG_SCENE_OCTREE_H */

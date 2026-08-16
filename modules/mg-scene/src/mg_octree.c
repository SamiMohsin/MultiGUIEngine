/**
 * @file mg_octree.c
 * @brief MultiGUIEngine 3D Spatial Octree Partitioning & Culling Engine implementation.
 */
#include "mg/scene/octree.h"
#include <stdlib.h>
#include <string.h>

static bool aabb_overlap(
    const mg_vec3_t* min1, const mg_vec3_t* max1,
    const mg_vec3_t* min2, const mg_vec3_t* max2
) {
    if (max1->x < min2->x || min1->x > max2->x) return false;
    if (max1->y < min2->y || min1->y > max2->y) return false;
    if (max1->z < min2->z || min1->z > max2->z) return false;
    return true;
}

static mg_octree_node_t* create_node(const mg_vec3_t* min, const mg_vec3_t* max) {
    mg_octree_node_t* node = (mg_octree_node_t*)calloc(1, sizeof(mg_octree_node_t));
    if (!node) return NULL;
    node->min = *min;
    node->max = *max;
    node->is_leaf = true;
    return node;
}

static void free_node(mg_octree_node_t* node) {
    if (!node) return;
    if (!node->is_leaf) {
        for (int i = 0; i < 8; ++i) {
            free_node(node->children[i]);
            node->children[i] = NULL;
        }
    }
    free(node);
}

static void subdivide_node(mg_octree_node_t* node) {
    if (!node || !node->is_leaf) return;

    mg_vec3_t mid = {
        (node->min.x + node->max.x) * 0.5f,
        (node->min.y + node->max.y) * 0.5f,
        (node->min.z + node->max.z) * 0.5f
    };

    for (int i = 0; i < 8; ++i) {
        mg_vec3_t c_min = {
            (i & 1) ? mid.x : node->min.x,
            (i & 2) ? mid.y : node->min.y,
            (i & 4) ? mid.z : node->min.z
        };
        mg_vec3_t c_max = {
            (i & 1) ? node->max.x : mid.x,
            (i & 2) ? node->max.y : mid.y,
            (i & 4) ? node->max.z : mid.z
        };
        node->children[i] = create_node(&c_min, &c_max);
    }
    node->is_leaf = false;
}

static bool insert_node(
    mg_octree_node_t* node,
    const mg_octree_item_t* item,
    uint32_t depth,
    uint32_t max_depth
) {
    if (!node || !aabb_overlap(&node->min, &node->max, &item->min, &item->max)) {
        return false;
    }

    if (node->is_leaf) {
        if (node->item_count < MG_OCTREE_MAX_ITEMS_PER_NODE || depth >= max_depth) {
            node->items[node->item_count++] = *item;
            return true;
        }

        subdivide_node(node);

        /* Push existing items to children where possible */
        for (uint32_t i = 0; i < node->item_count; ++i) {
            for (int c = 0; c < 8; ++c) {
                insert_node(node->children[c], &node->items[i], depth + 1, max_depth);
            }
        }
        node->item_count = 0;
    }

    bool inserted = false;
    for (int c = 0; c < 8; ++c) {
        if (insert_node(node->children[c], item, depth + 1, max_depth)) {
            inserted = true;
        }
    }
    return inserted;
}

void mg_octree_init(
    mg_octree_t* tree,
    const mg_vec3_t* world_min,
    const mg_vec3_t* world_max,
    uint32_t max_depth
) {
    if (!tree || !world_min || !world_max) return;
    tree->max_depth = max_depth ? max_depth : 6;
    tree->total_elements = 0;
    tree->root = create_node(world_min, world_max);
}

bool mg_octree_insert(
    mg_octree_t* tree,
    uint32_t entity_id,
    const mg_vec3_t* obj_min,
    const mg_vec3_t* obj_max
) {
    if (!tree || !tree->root || !obj_min || !obj_max) return false;

    mg_octree_item_t item;
    item.entity_id = entity_id;
    item.min = *obj_min;
    item.max = *obj_max;

    if (insert_node(tree->root, &item, 0, tree->max_depth)) {
        tree->total_elements++;
        return true;
    }
    return false;
}

static void query_node(
    const mg_octree_node_t* node,
    const mg_vec3_t* q_min,
    const mg_vec3_t* q_max,
    uint32_t* out_ids,
    size_t max_results,
    size_t* count
) {
    if (!node || !aabb_overlap(&node->min, &node->max, q_min, q_max)) {
        return;
    }

    if (node->is_leaf) {
        for (uint32_t i = 0; i < node->item_count; ++i) {
            if (*count >= max_results) return;
            if (aabb_overlap(&node->items[i].min, &node->items[i].max, q_min, q_max)) {
                /* Check duplicates in output list */
                bool exists = false;
                for (size_t k = 0; k < *count; ++k) {
                    if (out_ids[k] == node->items[i].entity_id) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    out_ids[(*count)++] = node->items[i].entity_id;
                }
            }
        }
    } else {
        for (int c = 0; c < 8; ++c) {
            if (*count >= max_results) return;
            query_node(node->children[c], q_min, q_max, out_ids, max_results, count);
        }
    }
}

size_t mg_octree_query_range(
    const mg_octree_t* tree,
    const mg_vec3_t* query_min,
    const mg_vec3_t* query_max,
    uint32_t* out_entity_ids,
    size_t max_results
) {
    if (!tree || !tree->root || !query_min || !query_max || !out_entity_ids || max_results == 0) {
        return 0;
    }

    size_t found = 0;
    query_node(tree->root, query_min, query_max, out_entity_ids, max_results, &found);
    return found;
}

void mg_octree_free(mg_octree_t* tree) {
    if (!tree) return;
    if (tree->root) {
        free_node(tree->root);
        tree->root = NULL;
    }
    tree->total_elements = 0;
}

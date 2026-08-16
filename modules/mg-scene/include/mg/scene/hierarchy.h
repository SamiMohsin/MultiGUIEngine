/**
 * @file hierarchy.h
 * @brief MultiGUIEngine Scene Transform Node Graph Hierarchy.
 */
#ifndef MG_SCENE_HIERARCHY_H
#define MG_SCENE_HIERARCHY_H

#include "mg/math/mat4.h"
#include "mg/math/vec3.h"
#include "mg/math/quat.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_MAX_NODE_CHILDREN 32

typedef struct mg_scene_node mg_scene_node_t;

struct mg_scene_node {
    char name[64];
    mg_vec3_t local_position;
    mg_quat_t local_rotation;
    mg_vec3_t local_scale;

    mg_mat4_t local_matrix;
    mg_mat4_t world_matrix;

    mg_scene_node_t* parent;
    mg_scene_node_t* children[MG_MAX_NODE_CHILDREN];
    uint32_t child_count;
};

/**
 * @brief Creates a scene transform node.
 */
mg_scene_node_t* mg_scene_node_create(const char* name);

/**
 * @brief Destroys a scene node and recursively all of its children.
 */
void mg_scene_node_destroy(mg_scene_node_t* node);

/**
 * @brief Attaches a child node to a parent node.
 */
bool mg_scene_node_add_child(mg_scene_node_t* parent, mg_scene_node_t* child);

/**
 * @brief Recursively propagates and updates local & world transform matrices down the node tree.
 */
void mg_scene_node_update_transforms(mg_scene_node_t* root, const mg_mat4_t* parent_world);

#ifdef __cplusplus
}
#endif

#endif /* MG_SCENE_HIERARCHY_H */

/**
 * @file mg_hierarchy.c
 * @brief MultiGUIEngine Scene Transform Node Graph Hierarchy implementation.
 */
#include "mg/scene/hierarchy.h"
#include <stdlib.h>
#include <string.h>

mg_scene_node_t* mg_scene_node_create(const char* name) {
    mg_scene_node_t* node = (mg_scene_node_t*)calloc(1, sizeof(mg_scene_node_t));
    if (node) {
        if (name) {
            strncpy(node->name, name, sizeof(node->name) - 1);
        }
        node->local_scale = (mg_vec3_t){ 1.0f, 1.0f, 1.0f };
        mg_quat_identity(&node->local_rotation);
        mg_mat4_identity(&node->local_matrix);
        mg_mat4_identity(&node->world_matrix);
    }
    return node;
}

void mg_scene_node_destroy(mg_scene_node_t* node) {
    if (!node) return;
    for (uint32_t i = 0; i < node->child_count; ++i) {
        mg_scene_node_destroy(node->children[i]);
    }
    free(node);
}

bool mg_scene_node_add_child(mg_scene_node_t* parent, mg_scene_node_t* child) {
    if (!parent || !child || parent->child_count >= MG_MAX_NODE_CHILDREN) return false;
    parent->children[parent->child_count++] = child;
    child->parent = parent;
    return true;
}

void mg_scene_node_update_transforms(mg_scene_node_t* root, const mg_mat4_t* parent_world) {
    if (!root) return;

    mg_mat4_t m_t, m_r, m_s, m_tr;
    mg_mat4_translation(&m_t, &root->local_position);
    mg_quat_to_mat4(&m_r, &root->local_rotation);
    mg_mat4_scaling(&m_s, &root->local_scale);

    mg_mat4_mul(&m_tr, &m_t, &m_r);
    mg_mat4_mul(&root->local_matrix, &m_tr, &m_s);

    if (parent_world) {
        mg_mat4_mul(&root->world_matrix, parent_world, &root->local_matrix);
    } else {
        mg_mat4_copy(&root->world_matrix, &root->local_matrix);
    }

    for (uint32_t i = 0; i < root->child_count; ++i) {
        mg_scene_node_update_transforms(root->children[i], &root->world_matrix);
    }
}

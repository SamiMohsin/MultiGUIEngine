/**
 * @file skeleton.h
 * @brief MultiGUIEngine 3D Skeletal Animation & Linear Blend Skinning (LBS).
 */
#ifndef MG_SCENE_SKELETON_H
#define MG_SCENE_SKELETON_H

#include "mg/math/mat4.h"
#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_MAX_BONES 32

typedef struct mg_bone {
    char name[32];
    int32_t parent_index;  /* -1 for root bone */
    mg_mat4_t local_transform;
    mg_mat4_t global_transform;
    mg_mat4_t inverse_bind_pose;
} mg_bone_t;

typedef struct mg_skeleton {
    mg_bone_t bones[MG_MAX_BONES];
    uint32_t bone_count;
    mg_mat4_t skin_palette[MG_MAX_BONES];
} mg_skeleton_t;

/**
 * @brief Initializes a skeleton armature.
 */
void mg_skeleton_init(mg_skeleton_t* skeleton);

/**
 * @brief Adds a bone to the skeleton.
 * @return Index of the created bone, or -1 on error.
 */
int32_t mg_skeleton_add_bone(
    mg_skeleton_t* skeleton,
    const char* name,
    int32_t parent_index,
    const mg_mat4_t* local_bind_pose
);

/**
 * @brief Sets a bone's local animation transform.
 */
void mg_skeleton_set_local_transform(
    mg_skeleton_t* skeleton,
    int32_t bone_index,
    const mg_mat4_t* local_transform
);

/**
 * @brief Evaluates global bone transforms and updates the skinning matrix palette.
 */
void mg_skeleton_update_palette(mg_skeleton_t* skeleton);

/**
 * @brief Transforms a vertex position using 4-bone Linear Blend Skinning (LBS).
 */
mg_vec3_t mg_skin_vertex_linear_blend(
    const mg_vec3_t* bind_pos,
    const uint8_t bone_indices[4],
    const float bone_weights[4],
    const mg_mat4_t* skin_palette
);

#ifdef __cplusplus
}
#endif

#endif /* MG_SCENE_SKELETON_H */

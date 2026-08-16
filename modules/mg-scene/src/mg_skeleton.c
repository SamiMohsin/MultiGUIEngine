/**
 * @file mg_skeleton.c
 * @brief MultiGUIEngine 3D Skeletal Animation & Linear Blend Skinning (LBS) implementation.
 */
#include "mg/scene/skeleton.h"
#include <string.h>

void mg_skeleton_init(mg_skeleton_t* skeleton) {
    if (!skeleton) return;
    memset(skeleton, 0, sizeof(mg_skeleton_t));
}

int32_t mg_skeleton_add_bone(
    mg_skeleton_t* skeleton,
    const char* name,
    int32_t parent_index,
    const mg_mat4_t* local_bind_pose
) {
    if (!skeleton || skeleton->bone_count >= MG_MAX_BONES) return -1;

    int32_t idx = (int32_t)skeleton->bone_count++;
    mg_bone_t* bone = &skeleton->bones[idx];

    if (name) {
        strncpy(bone->name, name, sizeof(bone->name) - 1);
    } else {
        bone->name[0] = '\0';
    }

    bone->parent_index = parent_index;

    if (local_bind_pose) {
        bone->local_transform = *local_bind_pose;
    } else {
        mg_mat4_identity(&bone->local_transform);
    }

    /* Compute Global Bind Pose */
    if (parent_index >= 0 && parent_index < idx) {
        mg_mat4_mul(&bone->global_transform, &skeleton->bones[parent_index].global_transform, &bone->local_transform);
    } else {
        bone->global_transform = bone->local_transform;
    }

    /* Compute Inverse Bind Pose */
    if (!mg_mat4_invert(&bone->inverse_bind_pose, &bone->global_transform)) {
        mg_mat4_identity(&bone->inverse_bind_pose);
    }

    /* Initial skin palette matrix */
    mg_mat4_identity(&skeleton->skin_palette[idx]);

    return idx;
}

void mg_skeleton_set_local_transform(
    mg_skeleton_t* skeleton,
    int32_t bone_index,
    const mg_mat4_t* local_transform
) {
    if (!skeleton || bone_index < 0 || (uint32_t)bone_index >= skeleton->bone_count || !local_transform) {
        return;
    }
    skeleton->bones[bone_index].local_transform = *local_transform;
}

void mg_skeleton_update_palette(mg_skeleton_t* skeleton) {
    if (!skeleton) return;

    for (uint32_t i = 0; i < skeleton->bone_count; ++i) {
        mg_bone_t* bone = &skeleton->bones[i];

        if (bone->parent_index >= 0 && (uint32_t)bone->parent_index < i) {
            mg_mat4_mul(&bone->global_transform, &skeleton->bones[bone->parent_index].global_transform, &bone->local_transform);
        } else {
            bone->global_transform = bone->local_transform;
        }

        /* Skinning Matrix = GlobalTransform * InverseBindPose */
        mg_mat4_mul(&skeleton->skin_palette[i], &bone->global_transform, &bone->inverse_bind_pose);
    }
}

mg_vec3_t mg_skin_vertex_linear_blend(
    const mg_vec3_t* bind_pos,
    const uint8_t bone_indices[4],
    const float bone_weights[4],
    const mg_mat4_t* skin_palette
) {
    if (!bind_pos || !bone_indices || !bone_weights || !skin_palette) {
        return (mg_vec3_t){ 0.0f, 0.0f, 0.0f };
    }

    mg_vec3_t skinned = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < 4; ++i) {
        float w = bone_weights[i];
        if (w > 0.0001f) {
            uint8_t bone_id = bone_indices[i];
            mg_vec3_t transformed;
            mg_mat4_transform_point(&transformed, &skin_palette[bone_id], bind_pos);

            skinned.x += transformed.x * w;
            skinned.y += transformed.y * w;
            skinned.z += transformed.z * w;
        }
    }

    return skinned;
}

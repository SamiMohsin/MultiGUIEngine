/**
 * @file main.c
 * @brief Example 28: 3D Skeletal Bone Armatures, Forward Kinematics & Linear Blend Skinning (LBS).
 */
#include "mg/scene/scene.h"
#include "mg/math/math.h"
#include <stdio.h>

#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main(void) {
    printf("=== Example 28: 3D Skeletal Animation & Linear Blend Skinning (LBS) ===\n");

    /* 1. Construct 3-Joint Armature (Root -> Shoulder -> Forearm) */
    mg_skeleton_t skeleton;
    mg_skeleton_init(&skeleton);

    /* Root Joint (at origin) */
    mg_mat4_t root_bind;
    mg_mat4_identity(&root_bind);
    int32_t root_bone = mg_skeleton_add_bone(&skeleton, "Root", -1, &root_bind);

    /* Shoulder Joint (offset +10.0 on Y) */
    mg_mat4_t shoulder_bind;
    mg_vec3_t shoulder_offset = { 0.0f, 10.0f, 0.0f };
    mg_mat4_translation(&shoulder_bind, &shoulder_offset);
    int32_t shoulder_bone = mg_skeleton_add_bone(&skeleton, "Shoulder", root_bone, &shoulder_bind);

    /* Forearm Joint (offset +10.0 on Y from Shoulder) */
    mg_mat4_t forearm_bind;
    mg_vec3_t forearm_offset = { 0.0f, 10.0f, 0.0f };
    mg_mat4_translation(&forearm_bind, &forearm_offset);
    int32_t forearm_bone = mg_skeleton_add_bone(&skeleton, "Forearm", shoulder_bone, &forearm_bind);

    assert(skeleton.bone_count == 3);
    printf("PASS: 3-Joint Armature constructed (Root -> Shoulder -> Forearm)\n");

    /* 2. Compute Initial Bind Pose Palette (should all be identity because no animation has been applied) */
    mg_skeleton_update_palette(&skeleton);
    mg_vec3_t bind_vertex = { 0.0f, 15.0f, 0.0f }; /* Midpoint vertex between Shoulder (y=10) and Forearm (y=20) */

    uint8_t bone_indices[4] = { (uint8_t)shoulder_bone, (uint8_t)forearm_bone, 0, 0 };
    float bone_weights[4]   = { 0.5f, 0.5f, 0.0f, 0.0f }; /* 50% Shoulder, 50% Forearm */

    mg_vec3_t skinned_bind = mg_skin_vertex_linear_blend(&bind_vertex, bone_indices, bone_weights, skeleton.skin_palette);
    assert(fabsf(skinned_bind.x - 0.0f) < 0.001f);
    assert(fabsf(skinned_bind.y - 15.0f) < 0.001f);
    printf("PASS: Bind Pose Identity Skinning verified at (%.2f, %.2f, %.2f)\n",
           skinned_bind.x, skinned_bind.y, skinned_bind.z);

    /* 3. Apply 90-degree Rotation around Z-axis on Shoulder Joint */
    mg_mat4_t shoulder_anim_rot;
    mg_vec3_t z_axis = { 0.0f, 0.0f, 1.0f };
    mg_mat4_rotation_axis(&shoulder_anim_rot, &z_axis, MG_PI * 0.5f); /* 90 degrees */

    mg_mat4_t shoulder_anim_trans;
    mg_mat4_translation(&shoulder_anim_trans, &shoulder_offset);

    mg_mat4_t shoulder_anim_local;
    mg_mat4_mul(&shoulder_anim_local, &shoulder_anim_trans, &shoulder_anim_rot);
    mg_skeleton_set_local_transform(&skeleton, shoulder_bone, &shoulder_anim_local);

    /* 4. Update Skinning Palette with Forward Kinematics */
    mg_skeleton_update_palette(&skeleton);

    /* 5. Evaluate Linear Blend Skinning for Deformed Vertex */
    mg_vec3_t skinned_deformed = mg_skin_vertex_linear_blend(&bind_vertex, bone_indices, bone_weights, skeleton.skin_palette);
    printf("PASS: 90-deg Arm Rotation Skinning -> Deformed Vertex: (%.2f, %.2f, %.2f)\n",
           skinned_deformed.x, skinned_deformed.y, skinned_deformed.z);
    /* Vertex rotated 90 degrees to the left (negative X) */
    assert(skinned_deformed.x < -4.0f);

    printf("PASS: Example 28: 3D Skeletal Animation & Skinning completed successfully\n");
    return 0;
}

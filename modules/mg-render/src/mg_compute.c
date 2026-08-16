/**
 * @file mg_compute.c
 * @brief MultiGUIEngine Compute Shader Acceleration & High-Throughput Parallel Pipeline implementation.
 */
#include "mg/render/compute.h"

void mg_compute_skinning_dispatch(
    const mg_vec3_t* in_bind_vertices,
    const uint8_t* in_bone_indices_4,
    const float* in_bone_weights_4,
    size_t vertex_count,
    const mg_mat4_t* bone_palette,
    mg_vec3_t* out_skinned_vertices
) {
    if (!in_bind_vertices || !in_bone_indices_4 || !in_bone_weights_4 ||
        !bone_palette || !out_skinned_vertices || vertex_count == 0) {
        return;
    }

    for (size_t v = 0; v < vertex_count; ++v) {
        const mg_vec3_t* bind_pos = &in_bind_vertices[v];
        const uint8_t* indices = &in_bone_indices_4[v * 4];
        const float* weights = &in_bone_weights_4[v * 4];

        mg_vec3_t skinned = { 0.0f, 0.0f, 0.0f };

        for (int i = 0; i < 4; ++i) {
            float w = weights[i];
            if (w > 0.0001f) {
                uint8_t bone_id = indices[i];
                mg_vec3_t transformed;
                mg_mat4_transform_point(&transformed, &bone_palette[bone_id], bind_pos);

                skinned.x += transformed.x * w;
                skinned.y += transformed.y * w;
                skinned.z += transformed.z * w;
            }
        }

        out_skinned_vertices[v] = skinned;
    }
}

void mg_compute_particles_dispatch(
    mg_vec3_t* positions,
    mg_vec3_t* velocities,
    float* lifetimes,
    size_t particle_count,
    float dt,
    const mg_vec3_t* gravity
) {
    if (!positions || !velocities || !lifetimes || particle_count == 0 || !gravity) {
        return;
    }

    float gx = gravity->x * dt;
    float gy = gravity->y * dt;
    float gz = gravity->z * dt;

    for (size_t p = 0; p < particle_count; ++p) {
        if (lifetimes[p] > 0.0f) {
            velocities[p].x += gx;
            velocities[p].y += gy;
            velocities[p].z += gz;

            positions[p].x += velocities[p].x * dt;
            positions[p].y += velocities[p].y * dt;
            positions[p].z += velocities[p].z * dt;

            lifetimes[p] -= dt;
            if (lifetimes[p] < 0.0f) {
                lifetimes[p] = 0.0f;
            }
        }
    }
}

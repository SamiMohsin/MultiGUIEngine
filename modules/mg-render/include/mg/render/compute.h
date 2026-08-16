/**
 * @file compute.h
 * @brief MultiGUIEngine Compute Shader Acceleration & High-Throughput Parallel Pipeline.
 */
#ifndef MG_RENDER_COMPUTE_H
#define MG_RENDER_COMPUTE_H

#include "mg/math/mat4.h"
#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dispatches a parallel matrix palette vertex skinning compute pass.
 */
void mg_compute_skinning_dispatch(
    const mg_vec3_t* in_bind_vertices,
    const uint8_t* in_bone_indices_4,
    const float* in_bone_weights_4,
    size_t vertex_count,
    const mg_mat4_t* bone_palette,
    mg_vec3_t* out_skinned_vertices
);

/**
 * @brief Dispatches a high-throughput particle physics compute pass.
 */
void mg_compute_particles_dispatch(
    mg_vec3_t* positions,
    mg_vec3_t* velocities,
    float* lifetimes,
    size_t particle_count,
    float dt,
    const mg_vec3_t* gravity
);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_COMPUTE_H */

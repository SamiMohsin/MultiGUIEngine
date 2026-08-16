/**
 * @file ssao.h
 * @brief MultiGUIEngine Screen-Space Ambient Occlusion (SSAO) & Contact Shadows Engine.
 */
#ifndef MG_RENDER_SSAO_H
#define MG_RENDER_SSAO_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_SSAO_MAX_KERNEL_SIZE 64

typedef struct mg_ssao_desc {
    uint32_t kernel_size;
    float radius;
    float bias;
    float power;
} mg_ssao_desc_t;

/**
 * @brief Generates hemisphere sample vectors scaled with quadratic falloff.
 */
void mg_ssao_generate_kernel(mg_vec3_t* out_samples, uint32_t count);

/**
 * @brief Computes Screen-Space Ambient Occlusion from depth and normal buffers.
 */
void mg_ssao_compute_pass(
    const float* depth_buffer,
    const mg_vec3_t* normal_buffer,
    uint32_t width,
    uint32_t height,
    const mg_ssao_desc_t* desc,
    const mg_vec3_t* kernel,
    float* out_occlusion_buffer
);

/**
 * @brief Bilateral depth-aware blur filter to smooth SSAO noise without blurring across depth edges.
 */
void mg_ssao_blur_pass(
    const float* in_occlusion,
    const float* depth_buffer,
    uint32_t width,
    uint32_t height,
    float* out_blurred_occlusion
);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_SSAO_H */

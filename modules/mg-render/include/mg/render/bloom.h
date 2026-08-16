/**
 * @file bloom.h
 * @brief MultiGUIEngine HDR Bloom, Bright-Pass Extraction & Gaussian Glow Post-Processing.
 */
#ifndef MG_RENDER_BLOOM_H
#define MG_RENDER_BLOOM_H

#include "mg/render/rhi.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_bloom_desc {
    float threshold;  /* Luminance threshold [0.0, 1.0], e.g. 0.7f */
    float intensity;  /* Bloom additive intensity multiplier, e.g. 1.2f */
    int blur_passes;  /* Number of separable Gaussian blur iterations, e.g. 2 */
} mg_bloom_desc_t;

/**
 * @brief Extracts HDR bright-pass pixels exceeding the luminance threshold.
 */
void mg_bloom_extract_bright_pass(
    const mg_color_t* src_buffer,
    mg_color_t* dst_bright_buffer,
    uint32_t width,
    uint32_t height,
    float threshold
);

/**
 * @brief Performs a 5-tap separable horizontal and vertical Gaussian blur pass.
 */
void mg_bloom_gaussian_blur(
    const mg_color_t* src_buffer,
    mg_color_t* dst_blurred_buffer,
    uint32_t width,
    uint32_t height
);

/**
 * @brief Composites bloom glow additively onto original scene buffer with saturation clamping.
 */
void mg_bloom_composite(
    const mg_color_t* scene_buffer,
    const mg_color_t* bloom_buffer,
    mg_color_t* dst_final_buffer,
    uint32_t width,
    uint32_t height,
    float intensity
);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_BLOOM_H */

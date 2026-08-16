/**
 * @file postprocess.h
 * @brief MultiGUIEngine Post-Processing Render Stack (ACES Tone Mapping, Bloom, FXAA).
 */
#ifndef MG_RENDER_POSTPROCESS_H
#define MG_RENDER_POSTPROCESS_H

#include "mg/render/rhi.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_postprocess_config {
    bool enable_bloom;
    float bloom_threshold;
    float bloom_intensity;
    bool enable_tonemapping;
    float exposure;
    bool enable_fxaa;
} mg_postprocess_config_t;

/**
 * @brief Applies fullscreen post-processing pipeline to frame buffer pixels.
 */
void mg_postprocess_apply(
    uint32_t* pixel_buffer,
    uint32_t width,
    uint32_t height,
    const mg_postprocess_config_t* config
);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_POSTPROCESS_H */

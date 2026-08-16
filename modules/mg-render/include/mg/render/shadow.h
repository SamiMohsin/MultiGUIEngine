/**
 * @file shadow.h
 * @brief MultiGUIEngine 3D Shadow Mapping & Percentage-Closer Filtering (PCF).
 */
#ifndef MG_RENDER_SHADOW_H
#define MG_RENDER_SHADOW_H

#include "mg/math/mat4.h"
#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_shadow_map {
    uint32_t width;
    uint32_t height;
    float* depth_buffer;
    mg_mat4_t light_view_proj;
    float depth_bias;
} mg_shadow_map_t;

/**
 * @brief Creates an in-memory shadow depth map.
 */
mg_shadow_map_t* mg_shadow_map_create(uint32_t width, uint32_t height);

/**
 * @brief Destroys a shadow depth map.
 */
void mg_shadow_map_destroy(mg_shadow_map_t* shadow_map);

/**
 * @brief Clears the shadow depth map to maximum depth (1.0).
 */
void mg_shadow_map_clear(mg_shadow_map_t* shadow_map);

/**
 * @brief Sets the light view-projection matrix for the shadow map.
 */
void mg_shadow_map_set_light_matrix(
    mg_shadow_map_t* shadow_map,
    const mg_mat4_t* light_view_proj
);

/**
 * @brief Rasterizes a 3D axis-aligned bounding box into the shadow depth buffer.
 */
void mg_shadow_rasterize_box(
    mg_shadow_map_t* sm,
    const mg_vec3_t* center,
    const mg_vec3_t* half_extents
);

/**
 * @brief Samples the shadow map with 3x3 Percentage-Closer Filtering (PCF).
 * @return Light visibility factor in [0.0, 1.0] (1.0 = fully lit, 0.0 = fully in shadow).
 */
float mg_shadow_sample_pcf(
    const mg_shadow_map_t* sm,
    const mg_vec3_t* world_pos
);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_SHADOW_H */

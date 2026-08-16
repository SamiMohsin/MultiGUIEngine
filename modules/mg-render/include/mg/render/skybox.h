/**
 * @file skybox.h
 * @brief MultiGUIEngine Skybox Environment Mapping and PBR Image-Based Lighting (IBL).
 */
#ifndef MG_RENDER_SKYBOX_H
#define MG_RENDER_SKYBOX_H

#include "mg/render/rhi.h"
#include "mg/math/vec3.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_cubemap_face {
    MG_CUBEMAP_POS_X = 0,
    MG_CUBEMAP_NEG_X,
    MG_CUBEMAP_POS_Y,
    MG_CUBEMAP_NEG_Y,
    MG_CUBEMAP_POS_Z,
    MG_CUBEMAP_NEG_Z,
    MG_CUBEMAP_FACE_COUNT
} mg_cubemap_face_t;

typedef struct mg_cubemap {
    uint32_t resolution;
    mg_color_t* faces[MG_CUBEMAP_FACE_COUNT];
} mg_cubemap_t;

typedef struct mg_pbr_material {
    mg_color_t albedo;
    float metallic;     /* [0.0, 1.0] */
    float roughness;    /* [0.0, 1.0] */
    float ao;           /* [0.0, 1.0] */
} mg_pbr_material_t;

/**
 * @brief Creates a cubemap with the given resolution.
 */
mg_cubemap_t* mg_cubemap_create(uint32_t resolution);

/**
 * @brief Destroys a cubemap.
 */
void mg_cubemap_destroy(mg_cubemap_t* cubemap);

/**
 * @brief Generates a procedural atmospheric skybox with a sun disc and horizon gradient.
 */
void mg_skybox_generate_procedural(
    mg_cubemap_t* cubemap,
    const mg_vec3_t* sun_dir,
    const mg_color_t* zenith_color,
    const mg_color_t* horizon_color
);

/**
 * @brief Samples a color from a cubemap along a 3D ray direction.
 */
mg_color_t mg_cubemap_sample(const mg_cubemap_t* cubemap, const mg_vec3_t* dir);

/**
 * @brief Evaluates PBR Image-Based Lighting (IBL) reflection color for a surface.
 */
mg_color_t mg_pbr_ibl_evaluate(
    const mg_cubemap_t* env_map,
    const mg_pbr_material_t* mat,
    const mg_vec3_t* normal,
    const mg_vec3_t* view_dir
);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_SKYBOX_H */

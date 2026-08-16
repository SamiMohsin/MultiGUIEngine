/**
 * @file backend_software.h
 * @brief CPU Software Rasterizer Backend (reference implementation for embedded & CI).
 */
#ifndef MG_RENDER_BACKEND_SOFTWARE_H
#define MG_RENDER_BACKEND_SOFTWARE_H

#include "mg/render/rhi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Software rasterizer vertex format.
 */
typedef struct mg_software_vertex {
    float x, y, z;       /**< 3D position. */
    float u, v;          /**< Texture coordinates. */
    float r, g, b, a;    /**< Vertex color. */
} mg_software_vertex_t;

/**
 * @brief Returns the software rasterizer RHI driver table.
 */
const mg_rhi_backend_t* mg_backend_software_get(void);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_BACKEND_SOFTWARE_H */

/**
 * @file batch2d.h
 * @brief High-performance batched 2D quad and sprite renderer.
 */
#ifndef MG_RENDER_BATCH2D_H
#define MG_RENDER_BATCH2D_H

#include "mg/render/rhi.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque 2D batcher handle */
typedef struct mg_batch2d mg_batch2d_t;

/**
 * @brief Standard 2D batcher vertex layout.
 */
typedef struct mg_vertex2d {
    float x, y, z;
    float u, v;
    float r, g, b, a;
} mg_vertex2d_t;

/**
 * @brief Creates a 2D batch renderer.
 * @param max_quads Maximum quads per batch (e.g. 1024).
 * @return Pointer to batcher, or NULL on failure.
 */
mg_batch2d_t* mg_batch2d_create(uint32_t max_quads);

/**
 * @brief Destroys a 2D batch renderer.
 * @param batch Pointer to batcher. Safe to pass NULL.
 */
void mg_batch2d_destroy(mg_batch2d_t* batch);

/**
 * @brief Begins a 2D rendering batch session.
 * @param batch Pointer to batcher.
 */
void mg_batch2d_begin(mg_batch2d_t* batch);

/**
 * @brief Appends a colored rectangle quad to the batch.
 * @param batch Pointer to batcher.
 * @param x Left position.
 * @param y Top position.
 * @param width Quad width.
 * @param height Quad height.
 * @param color RGBA color.
 */
void mg_batch2d_draw_quad(
    mg_batch2d_t* batch,
    float x,
    float y,
    float width,
    float height,
    const mg_color_t* color
);

/**
 * @brief Appends a textured sprite quad to the batch.
 * @param batch Pointer to batcher.
 * @param x Left position.
 * @param y Top position.
 * @param width Quad width.
 * @param height Quad height.
 * @param u0 Top-left U texture coordinate.
 * @param v0 Top-left V texture coordinate.
 * @param u1 Bottom-right U texture coordinate.
 * @param v1 Bottom-right V texture coordinate.
 * @param tint Tint color RGBA (can be NULL for white).
 * @param texture Texture handle.
 */
void mg_batch2d_draw_quad_textured(
    mg_batch2d_t* batch,
    float x,
    float y,
    float width,
    float height,
    float u0,
    float v0,
    float u1,
    float v1,
    const mg_color_t* tint,
    mg_texture_handle_t texture
);

/**
 * @brief Flushes pending geometry in the batch to the RHI.
 * @param batch Pointer to batcher.
 */
void mg_batch2d_flush(mg_batch2d_t* batch);

/**
 * @brief Ends the 2D rendering batch session, flushing any remaining geometry.
 * @param batch Pointer to batcher.
 */
void mg_batch2d_end(mg_batch2d_t* batch);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_BATCH2D_H */

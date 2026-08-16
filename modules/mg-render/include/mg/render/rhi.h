/**
 * @file rhi.h
 * @brief Backend-agnostic Render Hardware Interface (RHI).
 */
#ifndef MG_RENDER_RHI_H
#define MG_RENDER_RHI_H

#include "mg/math/math.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque resource handles (0 indicates invalid handle) */
typedef uint32_t mg_buffer_handle_t;
typedef uint32_t mg_texture_handle_t;
typedef uint32_t mg_pipeline_handle_t;

#define MG_INVALID_HANDLE 0

/**
 * @brief RGBA float color representation.
 */
typedef struct mg_color {
    float r;
    float g;
    float b;
    float a;
} mg_color_t;

/**
 * @brief Supported graphics backends.
 */
typedef enum mg_rhi_backend_type {
    MG_RHI_BACKEND_SOFTWARE = 0,
    MG_RHI_BACKEND_VULKAN,
    MG_RHI_BACKEND_METAL,
    MG_RHI_BACKEND_GLES
} mg_rhi_backend_type_t;

/**
 * @brief Buffer usage types.
 */
typedef enum mg_buffer_type {
    MG_BUFFER_TYPE_VERTEX = 0,
    MG_BUFFER_TYPE_INDEX,
    MG_BUFFER_TYPE_UNIFORM
} mg_buffer_type_t;

/**
 * @brief Buffer creation descriptor.
 */
typedef struct mg_buffer_desc {
    mg_buffer_type_t type;
    size_t size;
    const void* initial_data;
} mg_buffer_desc_t;

/**
 * @brief Texture formats.
 */
typedef enum mg_texture_format {
    MG_TEXTURE_FORMAT_RGBA8 = 0,
    MG_TEXTURE_FORMAT_RGB8,
    MG_TEXTURE_FORMAT_R8
} mg_texture_format_t;

/**
 * @brief Texture creation descriptor.
 */
typedef struct mg_texture_desc {
    uint32_t width;
    uint32_t height;
    mg_texture_format_t format;
    const void* initial_data;
} mg_texture_desc_t;

/**
 * @brief Pipeline configuration descriptor.
 */
typedef struct mg_pipeline_desc {
    bool depth_test;
    bool depth_write;
    bool blend_enable;
    bool wireframe;
} mg_pipeline_desc_t;

/**
 * @brief Draw command specification.
 */
typedef struct mg_draw_cmd {
    mg_pipeline_handle_t pipeline;
    mg_buffer_handle_t vertex_buffer;
    mg_buffer_handle_t index_buffer;
    mg_texture_handle_t texture;
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t first_vertex;
    uint32_t first_index;
} mg_draw_cmd_t;

/**
 * @brief RHI initialization descriptor.
 */
typedef struct mg_rhi_desc {
    mg_rhi_backend_type_t backend_type;
    uint32_t width;
    uint32_t height;
    bool vsync;
    bool offscreen;
} mg_rhi_desc_t;

/**
 * @brief Function pointer table for an RHI backend driver.
 */
typedef struct mg_rhi_backend {
    bool (*init)(const mg_rhi_desc_t* desc);
    void (*shutdown)(void);
    mg_buffer_handle_t (*create_buffer)(const mg_buffer_desc_t* desc);
    bool (*update_buffer)(mg_buffer_handle_t handle, size_t offset, const void* data, size_t size);
    void (*destroy_buffer)(mg_buffer_handle_t handle);
    mg_texture_handle_t (*create_texture)(const mg_texture_desc_t* desc);
    void (*destroy_texture)(mg_texture_handle_t handle);
    mg_pipeline_handle_t (*create_pipeline)(const mg_pipeline_desc_t* desc);
    void (*destroy_pipeline)(mg_pipeline_handle_t handle);
    void (*begin_frame)(const mg_color_t* clear_color);
    void (*draw)(const mg_draw_cmd_t* cmd);
    void (*end_frame)(void);
    bool (*read_pixels)(void* out_buffer, size_t buffer_size);
} mg_rhi_backend_t;

/* Global RHI API */

/**
 * @brief Initializes the active rendering hardware interface with the given backend.
 * @param desc Configuration descriptor.
 * @return True on success, false on failure.
 */
bool mg_rhi_init(const mg_rhi_desc_t* desc);

/**
 * @brief Shuts down the active RHI and frees all associated resources.
 */
void mg_rhi_shutdown(void);

/**
 * @brief Creates a GPU/CPU buffer.
 */
mg_buffer_handle_t mg_rhi_create_buffer(const mg_buffer_desc_t* desc);

/**
 * @brief Updates content in an existing buffer.
 */
bool mg_rhi_update_buffer(mg_buffer_handle_t handle, size_t offset, const void* data, size_t size);

/**
 * @brief Destroys a buffer.
 */
void mg_rhi_destroy_buffer(mg_buffer_handle_t handle);

/**
 * @brief Creates a 2D texture.
 */
mg_texture_handle_t mg_rhi_create_texture(const mg_texture_desc_t* desc);

/**
 * @brief Destroys a texture.
 */
void mg_rhi_destroy_texture(mg_texture_handle_t handle);

/**
 * @brief Creates a pipeline state.
 */
mg_pipeline_handle_t mg_rhi_create_pipeline(const mg_pipeline_desc_t* desc);

/**
 * @brief Destroys a pipeline state.
 */
void mg_rhi_destroy_pipeline(mg_pipeline_handle_t handle);

/**
 * @brief Begins a new rendering frame and clears framebuffers.
 * @param clear_color Clear color RGBA (can be NULL for default black).
 */
void mg_rhi_begin_frame(const mg_color_t* clear_color);

/**
 * @brief Submits a draw command.
 */
void mg_rhi_draw(const mg_draw_cmd_t* cmd);

/**
 * @brief Ends the current frame and presents/finalizes framebuffers.
 */
void mg_rhi_end_frame(void);

/**
 * @brief Reads back current color framebuffer pixels (RGBA8) for tests and screenshots.
 * @param out_buffer Output destination buffer.
 * @param buffer_size Size in bytes of out_buffer.
 * @return True on success, false on failure.
 */
bool mg_rhi_read_pixels(void* out_buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_RHI_H */

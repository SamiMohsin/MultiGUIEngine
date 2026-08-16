/**
 * @file mg_backend_software.c
 * @brief CPU Software Rasterizer implementation.
 */
#include "mg/render/backend_software.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MG_SW_MAX_BUFFERS 64
#define MG_SW_MAX_TEXTURES 64
#define MG_SW_MAX_PIPELINES 32

typedef struct {
    uint8_t* data;
    size_t size;
    mg_buffer_type_t type;
    bool is_active;
} sw_buffer_t;

typedef struct {
    uint8_t* data;
    uint32_t width;
    uint32_t height;
    mg_texture_format_t format;
    bool is_active;
} sw_texture_t;

typedef struct {
    mg_pipeline_desc_t desc;
    bool is_active;
} sw_pipeline_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t* color_buffer; /**< 0xAABBGGRR format */
    float* depth_buffer;    /**< [0.0, 1.0] */
    sw_buffer_t buffers[MG_SW_MAX_BUFFERS];
    sw_texture_t textures[MG_SW_MAX_TEXTURES];
    sw_pipeline_t pipelines[MG_SW_MAX_PIPELINES];
    bool initialized;
} sw_context_t;

static sw_context_t g_sw_ctx;

static inline uint32_t pack_rgba(float r, float g, float b, float a) {
    uint32_t ir = (uint32_t)(mg_clamp(r, 0.0f, 1.0f) * 255.0f);
    uint32_t ig = (uint32_t)(mg_clamp(g, 0.0f, 1.0f) * 255.0f);
    uint32_t ib = (uint32_t)(mg_clamp(b, 0.0f, 1.0f) * 255.0f);
    uint32_t ia = (uint32_t)(mg_clamp(a, 0.0f, 1.0f) * 255.0f);
    return (ia << 24) | (ib << 16) | (ig << 8) | ir;
}

static inline float edge_function(float ax, float ay, float bx, float by, float cx, float cy) {
    return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
}

static bool sw_init(const mg_rhi_desc_t* desc) {
    if (!desc || desc->width == 0 || desc->height == 0) {
        return false;
    }

    memset(&g_sw_ctx, 0, sizeof(sw_context_t));
    g_sw_ctx.width = desc->width;
    g_sw_ctx.height = desc->height;

    size_t pixel_count = (size_t)desc->width * desc->height;
    g_sw_ctx.color_buffer = (uint32_t*)malloc(pixel_count * sizeof(uint32_t));
    g_sw_ctx.depth_buffer = (float*)malloc(pixel_count * sizeof(float));

    if (!g_sw_ctx.color_buffer || !g_sw_ctx.depth_buffer) {
        free(g_sw_ctx.color_buffer);
        free(g_sw_ctx.depth_buffer);
        return false;
    }

    g_sw_ctx.initialized = true;
    return true;
}

static void sw_shutdown(void) {
    if (!g_sw_ctx.initialized) return;

    for (size_t i = 0; i < MG_SW_MAX_BUFFERS; ++i) {
        if (g_sw_ctx.buffers[i].is_active && g_sw_ctx.buffers[i].data) {
            free(g_sw_ctx.buffers[i].data);
        }
    }
    for (size_t i = 0; i < MG_SW_MAX_TEXTURES; ++i) {
        if (g_sw_ctx.textures[i].is_active && g_sw_ctx.textures[i].data) {
            free(g_sw_ctx.textures[i].data);
        }
    }

    free(g_sw_ctx.color_buffer);
    free(g_sw_ctx.depth_buffer);
    memset(&g_sw_ctx, 0, sizeof(sw_context_t));
}

static mg_buffer_handle_t sw_create_buffer(const mg_buffer_desc_t* desc) {
    if (!desc || desc->size == 0) return MG_INVALID_HANDLE;

    for (uint32_t i = 1; i < MG_SW_MAX_BUFFERS; ++i) {
        if (!g_sw_ctx.buffers[i].is_active) {
            void* data = malloc(desc->size);
            if (!data) return MG_INVALID_HANDLE;

            if (desc->initial_data) {
                memcpy(data, desc->initial_data, desc->size);
            } else {
                memset(data, 0, desc->size);
            }

            g_sw_ctx.buffers[i].data = (uint8_t*)data;
            g_sw_ctx.buffers[i].size = desc->size;
            g_sw_ctx.buffers[i].type = desc->type;
            g_sw_ctx.buffers[i].is_active = true;
            return i;
        }
    }
    return MG_INVALID_HANDLE;
}

static bool sw_update_buffer(mg_buffer_handle_t handle, size_t offset, const void* data, size_t size) {
    if (handle == MG_INVALID_HANDLE || handle >= MG_SW_MAX_BUFFERS || !data) return false;
    sw_buffer_t* buf = &g_sw_ctx.buffers[handle];
    if (!buf->is_active || (offset + size) > buf->size) return false;

    memcpy(buf->data + offset, data, size);
    return true;
}

static void sw_destroy_buffer(mg_buffer_handle_t handle) {
    if (handle != MG_INVALID_HANDLE && handle < MG_SW_MAX_BUFFERS) {
        if (g_sw_ctx.buffers[handle].is_active) {
            free(g_sw_ctx.buffers[handle].data);
            g_sw_ctx.buffers[handle].data = NULL;
            g_sw_ctx.buffers[handle].is_active = false;
        }
    }
}

static mg_texture_handle_t sw_create_texture(const mg_texture_desc_t* desc) {
    if (!desc || desc->width == 0 || desc->height == 0) return MG_INVALID_HANDLE;

    for (uint32_t i = 1; i < MG_SW_MAX_TEXTURES; ++i) {
        if (!g_sw_ctx.textures[i].is_active) {
            size_t bytes_per_pixel = (desc->format == MG_TEXTURE_FORMAT_RGBA8) ? 4 :
                                     (desc->format == MG_TEXTURE_FORMAT_RGB8) ? 3 : 1;
            size_t total_bytes = (size_t)desc->width * desc->height * bytes_per_pixel;
            void* data = malloc(total_bytes);
            if (!data) return MG_INVALID_HANDLE;

            if (desc->initial_data) {
                memcpy(data, desc->initial_data, total_bytes);
            } else {
                memset(data, 0, total_bytes);
            }

            g_sw_ctx.textures[i].data = (uint8_t*)data;
            g_sw_ctx.textures[i].width = desc->width;
            g_sw_ctx.textures[i].height = desc->height;
            g_sw_ctx.textures[i].format = desc->format;
            g_sw_ctx.textures[i].is_active = true;
            return i;
        }
    }
    return MG_INVALID_HANDLE;
}

static void sw_destroy_texture(mg_texture_handle_t handle) {
    if (handle != MG_INVALID_HANDLE && handle < MG_SW_MAX_TEXTURES) {
        if (g_sw_ctx.textures[handle].is_active) {
            free(g_sw_ctx.textures[handle].data);
            g_sw_ctx.textures[handle].data = NULL;
            g_sw_ctx.textures[handle].is_active = false;
        }
    }
}

static mg_pipeline_handle_t sw_create_pipeline(const mg_pipeline_desc_t* desc) {
    for (uint32_t i = 1; i < MG_SW_MAX_PIPELINES; ++i) {
        if (!g_sw_ctx.pipelines[i].is_active) {
            g_sw_ctx.pipelines[i].desc = desc ? *desc : (mg_pipeline_desc_t){0};
            g_sw_ctx.pipelines[i].is_active = true;
            return i;
        }
    }
    return MG_INVALID_HANDLE;
}

static void sw_destroy_pipeline(mg_pipeline_handle_t handle) {
    if (handle != MG_INVALID_HANDLE && handle < MG_SW_MAX_PIPELINES) {
        g_sw_ctx.pipelines[handle].is_active = false;
    }
}

static void sw_begin_frame(const mg_color_t* clear_color) {
    if (!g_sw_ctx.initialized) return;

    uint32_t clear_val = clear_color ? pack_rgba(clear_color->r, clear_color->g, clear_color->b, clear_color->a) : 0xFF000000;
    size_t pixel_count = (size_t)g_sw_ctx.width * g_sw_ctx.height;

    for (size_t i = 0; i < pixel_count; ++i) {
        g_sw_ctx.color_buffer[i] = clear_val;
        g_sw_ctx.depth_buffer[i] = 1.0f; /* Clear depth to 1.0 */
    }
}

static void sw_rasterize_triangle(
    const mg_software_vertex_t* v0,
    const mg_software_vertex_t* v1,
    const mg_software_vertex_t* v2,
    const sw_texture_t* tex,
    bool depth_test,
    bool depth_write
) {
    /* Screen space viewport transformation */
    float half_w = (float)g_sw_ctx.width * 0.5f;
    float half_h = (float)g_sw_ctx.height * 0.5f;

    float p0x = (v0->x + 1.0f) * half_w;
    float p0y = (1.0f - v0->y) * half_h; /* Vulkan Y-down */

    float p1x = (v1->x + 1.0f) * half_w;
    float p1y = (1.0f - v1->y) * half_h;

    float p2x = (v2->x + 1.0f) * half_w;
    float p2y = (1.0f - v2->y) * half_h;

    /* Area */
    float area = edge_function(p0x, p0y, p1x, p1y, p2x, p2y);
    if (fabsf(area) < 1e-5f) return;

    /* Bounding box */
    int min_x = (int)fmaxf(0.0f, floorf(fminf(p0x, fminf(p1x, p2x))));
    int max_x = (int)fminf((float)(g_sw_ctx.width - 1), ceilf(fmaxf(p0x, fmaxf(p1x, p2x))));
    int min_y = (int)fmaxf(0.0f, floorf(fminf(p0y, fminf(p1y, p2y))));
    int max_y = (int)fminf((float)(g_sw_ctx.height - 1), ceilf(fmaxf(p0y, fmaxf(p1y, p2y))));

    float inv_area = 1.0f / area;

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            float px = (float)x + 0.5f;
            float py = (float)y + 0.5f;

            float w0 = edge_function(p1x, p1y, p2x, p2y, px, py);
            float w1 = edge_function(p2x, p2y, p0x, p0y, px, py);
            float w2 = edge_function(p0x, p0y, p1x, p1y, px, py);

            /* Check winding */
            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                w0 *= inv_area;
                w1 *= inv_area;
                w2 *= inv_area;

                float z = w0 * v0->z + w1 * v1->z + w2 * v2->z;
                size_t pixel_idx = (size_t)y * g_sw_ctx.width + (size_t)x;

                if (depth_test && z > g_sw_ctx.depth_buffer[pixel_idx]) {
                    continue; /* Occluded */
                }

                if (depth_write) {
                    g_sw_ctx.depth_buffer[pixel_idx] = z;
                }

                /* Interpolate color */
                float r = w0 * v0->r + w1 * v1->r + w2 * v2->r;
                float g = w0 * v0->g + w1 * v1->g + w2 * v2->g;
                float b = w0 * v0->b + w1 * v1->b + w2 * v2->b;
                float a = w0 * v0->a + w1 * v1->a + w2 * v2->a;

                /* Texture sampling */
                if (tex != NULL && tex->is_active && tex->data != NULL) {
                    float u = w0 * v0->u + w1 * v1->u + w2 * v2->u;
                    float v = w0 * v0->v + w1 * v1->v + w2 * v2->v;
                    u = u - floorf(u);
                    v = v - floorf(v);

                    uint32_t tx = (uint32_t)(u * (float)(tex->width - 1));
                    uint32_t ty = (uint32_t)(v * (float)(tex->height - 1));
                    size_t tex_idx = ((size_t)ty * tex->width + tx) * 4;

                    float tr = (float)tex->data[tex_idx + 0] / 255.0f;
                    float tg = (float)tex->data[tex_idx + 1] / 255.0f;
                    float tb = (float)tex->data[tex_idx + 2] / 255.0f;
                    float ta = (float)tex->data[tex_idx + 3] / 255.0f;

                    r *= tr;
                    g *= tg;
                    b *= tb;
                    a *= ta;
                }

                g_sw_ctx.color_buffer[pixel_idx] = pack_rgba(r, g, b, a);
            }
        }
    }
}

static void sw_draw(const mg_draw_cmd_t* cmd) {
    if (!cmd || cmd->vertex_buffer == MG_INVALID_HANDLE || cmd->vertex_buffer >= MG_SW_MAX_BUFFERS) {
        return;
    }

    sw_buffer_t* vbuf = &g_sw_ctx.buffers[cmd->vertex_buffer];
    if (!vbuf->is_active || !vbuf->data) return;

    const mg_software_vertex_t* vertices = (const mg_software_vertex_t*)vbuf->data;
    sw_texture_t* tex = (cmd->texture != MG_INVALID_HANDLE && cmd->texture < MG_SW_MAX_TEXTURES) ?
                        &g_sw_ctx.textures[cmd->texture] : NULL;

    bool depth_test = false;
    bool depth_write = false;
    if (cmd->pipeline != MG_INVALID_HANDLE && cmd->pipeline < MG_SW_MAX_PIPELINES) {
        depth_test = g_sw_ctx.pipelines[cmd->pipeline].desc.depth_test;
        depth_write = g_sw_ctx.pipelines[cmd->pipeline].desc.depth_write;
    }

    if (cmd->index_buffer != MG_INVALID_HANDLE && cmd->index_buffer < MG_SW_MAX_BUFFERS) {
        sw_buffer_t* ibuf = &g_sw_ctx.buffers[cmd->index_buffer];
        if (ibuf->is_active && ibuf->data && cmd->index_count >= 3) {
            const uint32_t* indices = (const uint32_t*)ibuf->data;
            for (uint32_t i = 0; i + 2 < cmd->index_count; i += 3) {
                uint32_t i0 = indices[cmd->first_index + i];
                uint32_t i1 = indices[cmd->first_index + i + 1];
                uint32_t i2 = indices[cmd->first_index + i + 2];
                sw_rasterize_triangle(&vertices[i0], &vertices[i1], &vertices[i2], tex, depth_test, depth_write);
            }
        }
    } else if (cmd->vertex_count >= 3) {
        for (uint32_t i = 0; i + 2 < cmd->vertex_count; i += 3) {
            uint32_t i0 = cmd->first_vertex + i;
            uint32_t i1 = cmd->first_vertex + i + 1;
            uint32_t i2 = cmd->first_vertex + i + 2;
            sw_rasterize_triangle(&vertices[i0], &vertices[i1], &vertices[i2], tex, depth_test, depth_write);
        }
    }
}

static void sw_end_frame(void) {
    /* Frame ready */
}

static bool sw_read_pixels(void* out_buffer, size_t buffer_size) {
    if (!g_sw_ctx.initialized || !out_buffer) return false;
    size_t needed = (size_t)g_sw_ctx.width * g_sw_ctx.height * sizeof(uint32_t);
    if (buffer_size < needed) return false;

    memcpy(out_buffer, g_sw_ctx.color_buffer, needed);
    return true;
}

static const mg_rhi_backend_t g_backend_software_vtable = {
    .init = sw_init,
    .shutdown = sw_shutdown,
    .create_buffer = sw_create_buffer,
    .update_buffer = sw_update_buffer,
    .destroy_buffer = sw_destroy_buffer,
    .create_texture = sw_create_texture,
    .destroy_texture = sw_destroy_texture,
    .create_pipeline = sw_create_pipeline,
    .destroy_pipeline = sw_destroy_pipeline,
    .begin_frame = sw_begin_frame,
    .draw = sw_draw,
    .end_frame = sw_end_frame,
    .read_pixels = sw_read_pixels
};

const mg_rhi_backend_t* mg_backend_software_get(void) {
    return &g_backend_software_vtable;
}

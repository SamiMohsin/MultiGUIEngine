/**
 * @file mg_batch2d.c
 * @brief High-performance 2D quad and sprite batcher implementation.
 */
#include "mg/render/batch2d.h"
#include <stdlib.h>
#include <string.h>

struct mg_batch2d {
    uint32_t max_quads;
    uint32_t current_quads;

    mg_vertex2d_t* vertex_buffer;
    uint32_t* index_buffer;

    mg_buffer_handle_t rhi_vbuf;
    mg_buffer_handle_t rhi_ibuf;
    mg_pipeline_handle_t rhi_pipeline;

    mg_texture_handle_t current_texture;
    bool is_active;
};

mg_batch2d_t* mg_batch2d_create(uint32_t max_quads) {
    if (max_quads == 0) max_quads = 1024;

    mg_batch2d_t* b = (mg_batch2d_t*)malloc(sizeof(mg_batch2d_t));
    if (!b) return NULL;

    b->max_quads = max_quads;
    b->current_quads = 0;
    b->current_texture = MG_INVALID_HANDLE;
    b->is_active = false;

    size_t vertex_count = (size_t)max_quads * 4;
    size_t index_count = (size_t)max_quads * 6;

    b->vertex_buffer = (mg_vertex2d_t*)malloc(vertex_count * sizeof(mg_vertex2d_t));
    b->index_buffer = (uint32_t*)malloc(index_count * sizeof(uint32_t));

    if (!b->vertex_buffer || !b->index_buffer) {
        free(b->vertex_buffer);
        free(b->index_buffer);
        free(b);
        return NULL;
    }

    /* Initialize static index buffer pattern: (0, 1, 2, 2, 3, 0) for each quad */
    for (uint32_t q = 0; q < max_quads; ++q) {
        uint32_t v_base = q * 4;
        uint32_t i_base = q * 6;

        b->index_buffer[i_base + 0] = v_base + 0;
        b->index_buffer[i_base + 1] = v_base + 1;
        b->index_buffer[i_base + 2] = v_base + 2;
        b->index_buffer[i_base + 3] = v_base + 2;
        b->index_buffer[i_base + 4] = v_base + 3;
        b->index_buffer[i_base + 5] = v_base + 0;
    }

    /* Create RHI buffers */
    mg_buffer_desc_t vdesc = {
        .type = MG_BUFFER_TYPE_VERTEX,
        .size = vertex_count * sizeof(mg_vertex2d_t),
        .initial_data = NULL
    };
    b->rhi_vbuf = mg_rhi_create_buffer(&vdesc);

    mg_buffer_desc_t idesc = {
        .type = MG_BUFFER_TYPE_INDEX,
        .size = index_count * sizeof(uint32_t),
        .initial_data = b->index_buffer
    };
    b->rhi_ibuf = mg_rhi_create_buffer(&idesc);

    mg_pipeline_desc_t pdesc = {
        .depth_test = false,
        .depth_write = false,
        .blend_enable = true,
        .wireframe = false
    };
    b->rhi_pipeline = mg_rhi_create_pipeline(&pdesc);

    return b;
}

void mg_batch2d_destroy(mg_batch2d_t* batch) {
    if (!batch) return;

    mg_rhi_destroy_pipeline(batch->rhi_pipeline);
    mg_rhi_destroy_buffer(batch->rhi_ibuf);
    mg_rhi_destroy_buffer(batch->rhi_vbuf);

    free(batch->vertex_buffer);
    free(batch->index_buffer);
    free(batch);
}

void mg_batch2d_begin(mg_batch2d_t* batch) {
    if (!batch) return;
    batch->current_quads = 0;
    batch->current_texture = MG_INVALID_HANDLE;
    batch->is_active = true;
}

void mg_batch2d_flush(mg_batch2d_t* batch) {
    if (!batch || batch->current_quads == 0) return;

    size_t vertex_bytes = (size_t)batch->current_quads * 4 * sizeof(mg_vertex2d_t);
    mg_rhi_update_buffer(batch->rhi_vbuf, 0, batch->vertex_buffer, vertex_bytes);

    mg_draw_cmd_t cmd = {
        .pipeline = batch->rhi_pipeline,
        .vertex_buffer = batch->rhi_vbuf,
        .index_buffer = batch->rhi_ibuf,
        .texture = batch->current_texture,
        .vertex_count = batch->current_quads * 4,
        .index_count = batch->current_quads * 6,
        .first_vertex = 0,
        .first_index = 0
    };
    mg_rhi_draw(&cmd);

    batch->current_quads = 0;
}

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
) {
    if (!batch || !batch->is_active) return;

    /* Flush if texture changed or buffer is full */
    if (batch->current_texture != texture || batch->current_quads >= batch->max_quads) {
        mg_batch2d_flush(batch);
        batch->current_texture = texture;
    }

    float r = tint ? tint->r : 1.0f;
    float g = tint ? tint->g : 1.0f;
    float b = tint ? tint->b : 1.0f;
    float a = tint ? tint->a : 1.0f;

    uint32_t v_idx = batch->current_quads * 4;

    /* Top-left */
    batch->vertex_buffer[v_idx + 0] = (mg_vertex2d_t){ x, y, 0.0f, u0, v0, r, g, b, a };
    /* Top-right */
    batch->vertex_buffer[v_idx + 1] = (mg_vertex2d_t){ x + width, y, 0.0f, u1, v0, r, g, b, a };
    /* Bottom-right */
    batch->vertex_buffer[v_idx + 2] = (mg_vertex2d_t){ x + width, y + height, 0.0f, u1, v1, r, g, b, a };
    /* Bottom-left */
    batch->vertex_buffer[v_idx + 3] = (mg_vertex2d_t){ x, y + height, 0.0f, u0, v1, r, g, b, a };

    batch->current_quads++;
}

void mg_batch2d_draw_quad(
    mg_batch2d_t* batch,
    float x,
    float y,
    float width,
    float height,
    const mg_color_t* color
) {
    mg_batch2d_draw_quad_textured(batch, x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, color, MG_INVALID_HANDLE);
}

void mg_batch2d_end(mg_batch2d_t* batch) {
    if (!batch || !batch->is_active) return;
    mg_batch2d_flush(batch);
    batch->is_active = false;
}

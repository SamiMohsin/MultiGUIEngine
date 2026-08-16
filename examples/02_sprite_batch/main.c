/**
 * @file main.c
 * @brief Example 02: 2D Sprite/Quad Batch rendering with Software RHI.
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Running Example 02: Sprite Batch (Software RHI)...\n");

    const uint32_t width = 128;
    const uint32_t height = 128;

    mg_rhi_desc_t rhi_desc = {
        .backend_type = MG_RHI_BACKEND_SOFTWARE,
        .width = width,
        .height = height,
        .vsync = false,
        .offscreen = true
    };

    if (!mg_rhi_init(&rhi_desc)) {
        fprintf(stderr, "FAIL: mg_rhi_init failed\n");
        return 1;
    }

    /* Create a 4x4 checkerboard texture */
    uint32_t checker_pixels[16] = {
        0xFFFFFFFF, 0xFF0000FF, 0xFFFFFFFF, 0xFF0000FF,
        0xFF0000FF, 0xFFFFFFFF, 0xFF0000FF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFF0000FF, 0xFFFFFFFF, 0xFF0000FF,
        0xFF0000FF, 0xFFFFFFFF, 0xFF0000FF, 0xFFFFFFFF
    };
    mg_texture_desc_t tdesc = {
        .width = 4,
        .height = 4,
        .format = MG_TEXTURE_FORMAT_RGBA8,
        .initial_data = checker_pixels
    };
    mg_texture_handle_t tex = mg_rhi_create_texture(&tdesc);

    mg_batch2d_t* batch = mg_batch2d_create(256);
    if (!batch) {
        fprintf(stderr, "FAIL: mg_batch2d_create failed\n");
        mg_rhi_destroy_texture(tex);
        mg_rhi_shutdown();
        return 1;
    }

    mg_color_t clear_color = { 0.1f, 0.1f, 0.1f, 1.0f };
    mg_rhi_begin_frame(&clear_color);

    mg_batch2d_begin(batch);

    /* Draw colored background quad (normalized coords [-1, 1]) */
    mg_color_t green = { 0.0f, 1.0f, 0.0f, 1.0f };
    mg_batch2d_draw_quad(batch, -0.8f, -0.8f, 0.6f, 0.6f, &green);

    /* Draw textured quad */
    mg_color_t white = { 1.0f, 1.0f, 1.0f, 1.0f };
    mg_batch2d_draw_quad_textured(batch, 0.0f, 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 1.0f, 1.0f, &white, tex);

    mg_batch2d_end(batch);
    mg_rhi_end_frame();

    /* Read back pixels and verify */
    uint32_t pixels[128 * 128];
    if (!mg_rhi_read_pixels(pixels, sizeof(pixels))) {
        fprintf(stderr, "FAIL: read pixels\n");
        mg_batch2d_destroy(batch);
        mg_rhi_destroy_texture(tex);
        mg_rhi_shutdown();
        return 1;
    }

    mg_batch2d_destroy(batch);
    mg_rhi_destroy_texture(tex);
    mg_rhi_shutdown();

    printf("PASS: Example 02: Sprite batch rendered successfully\n");
    return 0;
}

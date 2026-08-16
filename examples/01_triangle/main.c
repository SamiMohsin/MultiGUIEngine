/**
 * @file main.c
 * @brief Example 01: Triangle rendering with Software RHI.
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Running Example 01: Triangle (Software RHI)...\n");

    const uint32_t width = 64;
    const uint32_t height = 64;

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

    mg_software_vertex_t triangle_vertices[3] = {
        {  0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }, /* Top (Red) */
        {  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }, /* Bottom Right (Green) */
        { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }  /* Bottom Left (Blue) */
    };

    mg_buffer_desc_t vdesc = {
        .type = MG_BUFFER_TYPE_VERTEX,
        .size = sizeof(triangle_vertices),
        .initial_data = triangle_vertices
    };
    mg_buffer_handle_t vbuf = mg_rhi_create_buffer(&vdesc);
    if (vbuf == MG_INVALID_HANDLE) {
        fprintf(stderr, "FAIL: create vertex buffer\n");
        mg_rhi_shutdown();
        return 1;
    }

    mg_color_t clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
    mg_rhi_begin_frame(&clear_color);

    mg_draw_cmd_t cmd = {
        .pipeline = MG_INVALID_HANDLE,
        .vertex_buffer = vbuf,
        .index_buffer = MG_INVALID_HANDLE,
        .texture = MG_INVALID_HANDLE,
        .vertex_count = 3,
        .index_count = 0,
        .first_vertex = 0,
        .first_index = 0
    };
    mg_rhi_draw(&cmd);
    mg_rhi_end_frame();

    /* Read back pixels and verify center pixel is colored */
    uint32_t pixels[64 * 64];
    if (!mg_rhi_read_pixels(pixels, sizeof(pixels))) {
        fprintf(stderr, "FAIL: read pixels\n");
        mg_rhi_destroy_buffer(vbuf);
        mg_rhi_shutdown();
        return 1;
    }

    uint32_t center_pixel = pixels[32 * 64 + 32];
    if (center_pixel == 0xFF000000 || (center_pixel & 0x00FFFFFF) == 0) {
        fprintf(stderr, "FAIL: center pixel was not rendered (val: 0x%08X)\n", center_pixel);
        mg_rhi_destroy_buffer(vbuf);
        mg_rhi_shutdown();
        return 1;
    }

    mg_rhi_destroy_buffer(vbuf);
    mg_rhi_shutdown();

    printf("PASS: Example 01: Triangle rendered successfully (center pixel: 0x%08X)\n", center_pixel);
    return 0;
}

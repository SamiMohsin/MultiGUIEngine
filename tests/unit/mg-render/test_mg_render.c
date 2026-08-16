/**
 * @file test_mg_render.c
 * @brief Comprehensive headless unit tests for mg-render (RHI & 2D Batcher).
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_rhi_lifecycle_and_buffers(void) {
    printf("Testing RHI lifecycle and buffer allocation...\n");

    mg_rhi_desc_t desc = {
        .backend_type = MG_RHI_BACKEND_SOFTWARE,
        .width = 32,
        .height = 32,
        .vsync = false,
        .offscreen = true
    };

    if (!mg_rhi_init(&desc)) {
        fprintf(stderr, "FAIL: mg_rhi_init failed\n");
        return 1;
    }

    float initial_data[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
    mg_buffer_desc_t bdesc = {
        .type = MG_BUFFER_TYPE_VERTEX,
        .size = sizeof(initial_data),
        .initial_data = initial_data
    };

    mg_buffer_handle_t buf = mg_rhi_create_buffer(&bdesc);
    if (buf == MG_INVALID_HANDLE) {
        fprintf(stderr, "FAIL: mg_rhi_create_buffer returned invalid handle\n");
        mg_rhi_shutdown();
        return 1;
    }

    float update_data[4] = { 5.0f, 6.0f, 7.0f, 8.0f };
    bool update_ok = mg_rhi_update_buffer(buf, 0, update_data, sizeof(update_data));
    if (!update_ok) {
        fprintf(stderr, "FAIL: mg_rhi_update_buffer failed\n");
        mg_rhi_destroy_buffer(buf);
        mg_rhi_shutdown();
        return 1;
    }

    mg_rhi_destroy_buffer(buf);
    mg_rhi_shutdown();
    printf("PASS: RHI lifecycle and buffers\n");
    return 0;
}

static int test_software_rasterization(void) {
    printf("Testing software rasterizer triangle output...\n");

    const uint32_t w = 64, h = 64;
    mg_rhi_desc_t desc = {
        .backend_type = MG_RHI_BACKEND_SOFTWARE,
        .width = w,
        .height = h,
        .vsync = false,
        .offscreen = true
    };

    if (!mg_rhi_init(&desc)) return 1;

    mg_software_vertex_t tri[3] = {
        {  0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
    };
    mg_buffer_desc_t bdesc = {
        .type = MG_BUFFER_TYPE_VERTEX,
        .size = sizeof(tri),
        .initial_data = tri
    };
    mg_buffer_handle_t vbuf = mg_rhi_create_buffer(&bdesc);

    mg_color_t black = { 0.0f, 0.0f, 0.0f, 1.0f };
    mg_rhi_begin_frame(&black);

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

    uint32_t pixels[64 * 64];
    mg_rhi_read_pixels(pixels, sizeof(pixels));

    /* Corner pixel should be black background (0xFF000000) */
    uint32_t corner = pixels[0];
    if (corner != 0xFF000000) {
        fprintf(stderr, "FAIL: background corner not black (0x%08X)\n", corner);
        mg_rhi_destroy_buffer(vbuf);
        mg_rhi_shutdown();
        return 1;
    }

    /* Center pixel (32, 32) should be inside triangle */
    uint32_t center = pixels[32 * 64 + 32];
    if (center == 0xFF000000 || (center & 0x00FFFFFF) == 0) {
        fprintf(stderr, "FAIL: triangle center not drawn (0x%08X)\n", center);
        mg_rhi_destroy_buffer(vbuf);
        mg_rhi_shutdown();
        return 1;
    }

    mg_rhi_destroy_buffer(vbuf);
    mg_rhi_shutdown();
    printf("PASS: software rasterizer triangle\n");
    return 0;
}

static int test_batch2d(void) {
    printf("Testing 2D quad batcher...\n");

    const uint32_t w = 64, h = 64;
    mg_rhi_desc_t desc = {
        .backend_type = MG_RHI_BACKEND_SOFTWARE,
        .width = w,
        .height = h,
        .vsync = false,
        .offscreen = true
    };
    if (!mg_rhi_init(&desc)) return 1;

    mg_batch2d_t* batch = mg_batch2d_create(64);
    if (!batch) {
        mg_rhi_shutdown();
        return 1;
    }

    mg_color_t clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
    mg_rhi_begin_frame(&clear_color);

    mg_batch2d_begin(batch);

    /* Draw full screen quad (red) */
    mg_color_t red = { 1.0f, 0.0f, 0.0f, 1.0f };
    mg_batch2d_draw_quad(batch, -1.0f, -1.0f, 2.0f, 2.0f, &red);

    mg_batch2d_end(batch);
    mg_rhi_end_frame();

    uint32_t pixels[64 * 64];
    mg_rhi_read_pixels(pixels, sizeof(pixels));

    /* Center pixel should be red (0xFF0000FF in AABBGGRR) */
    uint32_t center = pixels[32 * 64 + 32];
    uint8_t r = (uint8_t)(center & 0xFF);
    if (r < 250) {
        fprintf(stderr, "FAIL: 2D batch red quad failed (pixel: 0x%08X, r=%d)\n", center, r);
        mg_batch2d_destroy(batch);
        mg_rhi_shutdown();
        return 1;
    }

    mg_batch2d_destroy(batch);
    mg_rhi_shutdown();
    printf("PASS: 2D quad batcher\n");
    return 0;
}

int main(void) {
    if (test_rhi_lifecycle_and_buffers() != 0) return 1;
    if (test_software_rasterization() != 0) return 1;
    if (test_batch2d() != 0) return 1;

    printf("All mg-render unit tests passed successfully!\n");
    return 0;
}

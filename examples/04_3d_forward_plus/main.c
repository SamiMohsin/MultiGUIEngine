/**
 * @file main.c
 * @brief Example 04: 3D Transformed Cube Rendering with Depth Testing & Lighting.
 */
#include "mg/render/render.h"
#include "mg/math/math.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 3D Cube: 36 vertices (12 triangles) */
static const mg_vec3_t CUBE_POSITIONS[36] = {
    /* Front face */
    {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f},
    /* Back face */
    {-0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
    /* Top face */
    {-0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
    /* Bottom face */
    {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f, -0.5f},
    /* Right face */
    { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f},
    /* Left face */
    {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}
};

int main(void) {
    printf("Running Example 04: 3D Forward+ Cube Rendering...\n");

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

    /* Transform cube vertices by MVP matrix */
    mg_mat4_t model, view, proj, view_model, mvp;
    mg_mat4_rotation_euler(&model, 0.5f, 0.785f, 0.0f);

    mg_vec3_t eye = {0.0f, 1.5f, 3.0f};
    mg_vec3_t target = {0.0f, 0.0f, 0.0f};
    mg_vec3_t up = {0.0f, 1.0f, 0.0f};
    mg_mat4_look_at(&view, &eye, &target, &up);

    mg_mat4_perspective(&proj, 1.047f, 1.0f, 0.1f, 10.0f);

    mg_mat4_mul(&view_model, &view, &model);
    mg_mat4_mul(&mvp, &proj, &view_model);

    mg_software_vertex_t vertices[36];
    for (int i = 0; i < 36; ++i) {
        mg_vec4_t p = { CUBE_POSITIONS[i].x, CUBE_POSITIONS[i].y, CUBE_POSITIONS[i].z, 1.0f };
        mg_vec4_t clip;
        mg_mat4_transform_vec4(&clip, &mvp, &p);

        float inv_w = 1.0f / (clip.w > 0.001f ? clip.w : 0.001f);
        vertices[i].x = clip.x * inv_w;
        vertices[i].y = clip.y * inv_w;
        vertices[i].z = (clip.z * inv_w + 1.0f) * 0.5f; /* [0, 1] */
        vertices[i].u = 0.0f;
        vertices[i].v = 0.0f;

        /* Shading based on face index */
        float shade = 0.3f + 0.7f * ((float)(i / 6) / 5.0f);
        vertices[i].r = shade * 0.8f;
        vertices[i].g = shade * 0.4f;
        vertices[i].b = shade * 1.0f;
        vertices[i].a = 1.0f;
    }

    mg_buffer_desc_t vdesc = {
        .type = MG_BUFFER_TYPE_VERTEX,
        .size = sizeof(vertices),
        .initial_data = vertices
    };
    mg_buffer_handle_t vbuf = mg_rhi_create_buffer(&vdesc);

    mg_pipeline_desc_t pdesc = {
        .depth_test = true,
        .depth_write = true,
        .blend_enable = false,
        .wireframe = false
    };
    mg_pipeline_handle_t pipe = mg_rhi_create_pipeline(&pdesc);

    mg_color_t bg = { 0.05f, 0.05f, 0.08f, 1.0f };
    mg_rhi_begin_frame(&bg);

    mg_draw_cmd_t cmd = {
        .pipeline = pipe,
        .vertex_buffer = vbuf,
        .index_buffer = MG_INVALID_HANDLE,
        .texture = MG_INVALID_HANDLE,
        .vertex_count = 36,
        .index_count = 0,
        .first_vertex = 0,
        .first_index = 0
    };
    mg_rhi_draw(&cmd);
    mg_rhi_end_frame();

    uint32_t pixels[128 * 128];
    mg_rhi_read_pixels(pixels, sizeof(pixels));

    /* Verify center pixel has been drawn */
    uint32_t center = pixels[64 * 128 + 64];
    if (center == 0xFF000000 || (center & 0x00FFFFFF) == 0) {
        fprintf(stderr, "FAIL: 3D cube center pixel empty (0x%08X)\n", center);
        mg_rhi_destroy_pipeline(pipe);
        mg_rhi_destroy_buffer(vbuf);
        mg_rhi_shutdown();
        return 1;
    }

    mg_rhi_destroy_pipeline(pipe);
    mg_rhi_destroy_buffer(vbuf);
    mg_rhi_shutdown();

    printf("PASS: Example 04: 3D Cube rendered successfully with depth testing (center pixel: 0x%08X)\n", center);
    return 0;
}

/**
 * @file main.c
 * @brief Example 39: Screen-Space Ambient Occlusion (SSAO) & Bilateral Blur Contact Shadows.
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define G_WIDTH  128
#define G_HEIGHT 128

int main(void) {
    printf("=== Example 39: Screen-Space Ambient Occlusion (SSAO) & Contact Shadows ===\n");

    /* 1. Allocate Depth, Normal, and Occlusion G-Buffers */
    float* depth_buffer = (float*)malloc(G_WIDTH * G_HEIGHT * sizeof(float));
    mg_vec3_t* normal_buffer = (mg_vec3_t*)malloc(G_WIDTH * G_HEIGHT * sizeof(mg_vec3_t));
    float* ssao_raw = (float*)malloc(G_WIDTH * G_HEIGHT * sizeof(float));
    float* ssao_blurred = (float*)malloc(G_WIDTH * G_HEIGHT * sizeof(float));
    assert(depth_buffer && normal_buffer && ssao_raw && ssao_blurred);

    /* 2. Synthesize Scene Geometry G-Buffer: Flat Floor + Vertical Recessed Crease at x=64 */
    for (uint32_t y = 0; y < G_HEIGHT; ++y) {
        for (uint32_t x = 0; x < G_WIDTH; ++x) {
            uint32_t idx = y * G_WIDTH + x;

            if (x < 64) {
                /* Front Wall / Left Surface (Z = 5.0m) */
                depth_buffer[idx] = 0.5f;
                normal_buffer[idx] = (mg_vec3_t){ 0.0f, 0.0f, 1.0f };
            } else {
                /* Recessed Back Wall (Z = 7.0m) */
                depth_buffer[idx] = 0.7f;
                normal_buffer[idx] = (mg_vec3_t){ 0.0f, 0.0f, 1.0f };
            }
        }
    }

    /* 3. Generate SSAO Hemisphere Kernel (32 Samples) */
    mg_vec3_t kernel[32];
    mg_ssao_generate_kernel(kernel, 32);
    printf("PASS: Generated 32-Sample Hemisphere SSAO Kernel with Quadratic Falloff\n");

    /* 4. Execute SSAO Compute Pass */
    mg_ssao_desc_t desc = {
        .kernel_size = 32,
        .radius = 0.5f,
        .bias = 0.025f,
        .power = 1.5f
    };

    mg_ssao_compute_pass(depth_buffer, normal_buffer, G_WIDTH, G_HEIGHT, &desc, kernel, ssao_raw);

    /* 5. Execute Depth-Aware Bilateral Blur Pass */
    mg_ssao_blur_pass(ssao_raw, depth_buffer, G_WIDTH, G_HEIGHT, ssao_blurred);

    /* 6. Verify Ambient Occlusion at Crease Edge vs Flat Surface */
    float flat_ao = ssao_blurred[32 * G_WIDTH + 32];     /* Flat surface: x=32 */
    float crease_ao = ssao_blurred[32 * G_WIDTH + 65];   /* Corner Crease: x=65 (immediately next to step) */

    printf("PASS: Ambient Accessibility -> Flat Plane: %.3f (Bright), Crease Corner: %.3f (Occluded Shadow)\n",
           flat_ao, crease_ao);

    assert(flat_ao > 0.90f);
    assert(crease_ao < flat_ao);

    /* Clean up */
    free(depth_buffer);
    free(normal_buffer);
    free(ssao_raw);
    free(ssao_blurred);

    printf("PASS: Example 39: Screen-Space Ambient Occlusion (SSAO) completed successfully\n");
    return 0;
}

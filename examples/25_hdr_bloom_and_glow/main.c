/**
 * @file main.c
 * @brief Example 25: HDR Bloom, Bright-Pass Filtering & Gaussian Glow Post-Processing.
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define WIDTH  64
#define HEIGHT 64

int main(void) {
    printf("=== Example 25: HDR Bloom Filter & Emissive Glow Post-Processing ===\n");

    /* 1. Allocate Scene Framebuffers (64x64) */
    size_t total_pixels = WIDTH * HEIGHT;
    mg_color_t* scene_buffer = (mg_color_t*)calloc(total_pixels, sizeof(mg_color_t));
    mg_color_t* bright_buffer = (mg_color_t*)calloc(total_pixels, sizeof(mg_color_t));
    mg_color_t* blurred_buffer = (mg_color_t*)calloc(total_pixels, sizeof(mg_color_t));
    mg_color_t* final_buffer = (mg_color_t*)calloc(total_pixels, sizeof(mg_color_t));

    assert(scene_buffer && bright_buffer && blurred_buffer && final_buffer);

    /* 2. Render Scene with Dark Background and Ultra-Bright Neon Emitter in the center (x: 28-36, y: 28-36) */
    for (uint32_t y = 0; y < HEIGHT; ++y) {
        for (uint32_t x = 0; x < WIDTH; ++x) {
            if (x >= 28 && x <= 36 && y >= 28 && y <= 36) {
                scene_buffer[y * WIDTH + x] = (mg_color_t){ 1.0f, 0.25f, 0.25f, 1.0f }; /* High-intensity neon emitter (bright red) */
            } else {
                scene_buffer[y * WIDTH + x] = (mg_color_t){ 0.05f, 0.05f, 0.08f, 1.0f }; /* Dim ambient background */
            }
        }
    }
    printf("PASS: Scene Buffer generated with ultra-bright neon core at center\n");

    /* 3. Extract HDR Bright-Pass Pixels (Threshold = 0.2) */
    mg_bloom_extract_bright_pass(scene_buffer, bright_buffer, WIDTH, HEIGHT, 0.2f);
    assert(bright_buffer[32 * WIDTH + 32].r > 0.2f); /* Core is bright */
    assert(bright_buffer[0].r == 0.0f);              /* Background rejected */
    printf("PASS: Bright-Pass Filter extracted emissive core and rejected ambient pixels\n");


    /* 4. Perform Separable Gaussian Blur on Bright Pass */
    mg_bloom_gaussian_blur(bright_buffer, blurred_buffer, WIDTH, HEIGHT);
    /* Verify Gaussian glow halo expanded outside core (e.g. at x=26, y=32) */
    mg_color_t glow_pixel = blurred_buffer[32 * WIDTH + 26];
    assert(glow_pixel.r > 0.0f);
    printf("PASS: Gaussian Blur expanded smooth glow halo (Halo R-channel: %.3f)\n", glow_pixel.r);

    /* 5. Composite Bloom Additively onto Scene */
    mg_bloom_composite(scene_buffer, blurred_buffer, final_buffer, WIDTH, HEIGHT, 1.2f);
    mg_color_t comp_center = final_buffer[32 * WIDTH + 32];
    mg_color_t comp_halo = final_buffer[32 * WIDTH + 26];
    printf("PASS: Final Composited Pixels -> Core: (%.2f, %.2f, %.2f), Halo: (%.2f, %.2f, %.2f)\n",
           comp_center.r, comp_center.g, comp_center.b, comp_halo.r, comp_halo.g, comp_halo.b);

    /* Cleanup */
    free(scene_buffer);
    free(bright_buffer);
    free(blurred_buffer);
    free(final_buffer);

    printf("PASS: Example 25: HDR Bloom & Glow Post-Processing completed successfully\n");
    return 0;
}

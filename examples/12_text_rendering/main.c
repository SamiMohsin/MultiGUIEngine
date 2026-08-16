/**
 * @file main.c
 * @brief Example 12: High-Performance Text Rendering and Font Atlas Showcase.
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 12: Text Rendering and Font Atlas Showcase ===\n");

    /* 1. Test Text Measurement */
    const char* sample_text = "MultiGUIEngine\nHigh-Performance C99 Engine";
    mg_vec2_t bounds = mg_font_measure_text(sample_text, 1.0f);
    assert(bounds.x > 0.0f);
    assert(bounds.y > 0.0f);
    printf("PASS: Text Bounds Measured: %.1f x %.1f px\n", bounds.x, bounds.y);

    /* 2. Test 2D Batch Text Drawing */
    mg_batch2d_t* batch = mg_batch2d_create(1024);
    assert(batch != NULL);

    mg_batch2d_begin(batch);
    mg_color_t color_title = { 1.0f, 0.8f, 0.2f, 1.0f };
    mg_font_draw_text(batch, "ASTROGUARD: SECTOR 4", 20.0f, 30.0f, 1.5f, &color_title);

    mg_color_t color_body = { 0.9f, 0.9f, 0.9f, 1.0f };
    mg_font_draw_text(batch, "Score: 14,500 | Shields: 100%", 20.0f, 70.0f, 1.0f, &color_body);
    mg_batch2d_end(batch);

    printf("PASS: Text Glyph Quads Batched successfully\n");
    mg_batch2d_destroy(batch);

    printf("PASS: Example 12: Text Rendering and Font Atlas completed successfully\n");
    return 0;
}


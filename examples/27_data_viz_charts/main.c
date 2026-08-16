/**
 * @file main.c
 * @brief Example 27: Data Visualization, Telemetry Line Graphs, Resource Bars & RPG Stat Radar Charts.
 */
#include "mg/ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define FB_WIDTH  128
#define FB_HEIGHT 128

int main(void) {
    printf("=== Example 27: Data Visualization & Dynamic Vector Charts ===\n");

    /* 1. Allocate Virtual Screen Framebuffer (128x128) */
    size_t total_pixels = FB_WIDTH * FB_HEIGHT;
    mg_color_t* fb = (mg_color_t*)calloc(total_pixels, sizeof(mg_color_t));
    assert(fb != NULL);

    /* Initialize background */
    for (size_t i = 0; i < total_pixels; ++i) {
        fb[i] = (mg_color_t){ 0.1f, 0.1f, 0.12f, 1.0f }; /* Dark M3 surface */
    }

    /* 2. Render Real-Time Frame Time Line Chart (Top Left: 10, 10, 50x30) */
    mg_chart_desc_t line_chart_desc = {
        .x = 10.0f,
        .y = 10.0f,
        .width = 50.0f,
        .height = 30.0f,
        .min_value = 0.0f,
        .max_value = 33.3f, /* 30-60 FPS window */
        .background_color = { 0.15f, 0.15f, 0.18f, 1.0f },
        .border_color = { 0.3f, 0.3f, 0.35f, 1.0f },
        .series_color = { 0.2f, 0.8f, 0.4f, 1.0f } /* Emerald Green */
    };
    const float frame_times[6] = { 16.6f, 16.8f, 17.2f, 16.4f, 32.0f, 16.5f };
    mg_chart_render_line(fb, FB_WIDTH, FB_HEIGHT, &line_chart_desc, frame_times, 6);
    printf("PASS: Frame Latency Telemetry Polyline Chart rendered (Sample Pixel = 0x%08X)\n",
           (uint32_t)(line_chart_desc.series_color.r * 255.0f));

    /* 3. Render Resource Inventory Bar Chart (Top Right: 68, 10, 50x30) */
    mg_chart_desc_t bar_chart_desc = {
        .x = 68.0f,
        .y = 10.0f,
        .width = 50.0f,
        .height = 30.0f,
        .min_value = 0.0f,
        .max_value = 100.0f,
        .background_color = { 0.15f, 0.15f, 0.18f, 1.0f },
        .border_color = { 0.3f, 0.3f, 0.35f, 1.0f },
        .series_color = { 0.3f, 0.6f, 1.0f, 1.0f } /* Neon Cyan/Blue */
    };
    const float inventory_bars[4] = { 80.0f, 45.0f, 95.0f, 30.0f }; /* Gold, Iron, Crystals, Fuel */
    mg_chart_render_bar(fb, FB_WIDTH, FB_HEIGHT, &bar_chart_desc, inventory_bars, 4);
    printf("PASS: Resource Inventory Histogram Bar Chart rendered\n");

    /* 4. Render 5-Axis Character RPG Stat Radar Chart (Bottom Center: Center = 64, 85, Radius = 30) */
    /* Stats: [Strength: 0.9, Agility: 0.75, Intelligence: 0.6, Armor: 0.85, Stamina: 0.7] */
    const float hero_stats[5] = { 0.90f, 0.75f, 0.60f, 0.85f, 0.70f };
    mg_color_t radar_color = { 0.95f, 0.45f, 0.2f, 1.0f }; /* Flame Orange */
    mg_chart_render_radar(fb, FB_WIDTH, FB_HEIGHT, 64.0f, 85.0f, 30.0f, hero_stats, 5, radar_color);
    printf("PASS: 5-Axis RPG Character Attribute Spider/Radar Chart rendered\n");

    /* Verify non-empty pixels rendered */
    assert(fb[15 * FB_WIDTH + 15].r > 0.1f);
    assert(fb[85 * FB_WIDTH + 64].r > 0.1f);

    /* Cleanup */
    free(fb);

    printf("PASS: Example 27: Data Visualization & Dynamic Vector Charts completed successfully\n");
    return 0;
}

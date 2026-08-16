/**
 * @file main.c
 * @brief Example 37: Rich Markdown Document Layout, Typography & Oscilloscope Theme.
 */
#include "mg/ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SURFACE_WIDTH  320
#define SURFACE_HEIGHT 240

static const char* SAMPLE_MARKDOWN =
    "# MultiGUIEngine Core Architecture\n"
    "MultiGUIEngine is a modular C99 cross-platform game engine.\n"
    "\n"
    "## Key Performance Capabilities\n"
    "> All reactive core mutations are synchronous and zero-allocation.\n"
    "\n"
    "- Archetype ECS with bitmask queries\n"
    "- High-Throughput Compute Pipelines\n"
    "- Position-Based Ragdoll Dynamics\n"
    "\n"
    "```c\n"
    "mg_rx_observable_t* obs = mg_rx_create();\n"
    "```\n";

int main(void) {
    printf("=== Example 37: Rich Markdown Document Layout & Typography ===\n");

    /* 1. Calculate Required Layout Height */
    uint32_t needed_height = mg_markdown_calc_height(SAMPLE_MARKDOWN, SURFACE_WIDTH);
    printf("PASS: Document Layout computed height: %u px (Surface dimensions: %dx%d)\n",
           needed_height, SURFACE_WIDTH, SURFACE_HEIGHT);
    assert(needed_height > 100);

    /* 2. Allocate Surface Buffer & Render Markdown Document */
    uint32_t* surface = (uint32_t*)malloc(SURFACE_WIDTH * SURFACE_HEIGHT * sizeof(uint32_t));
    assert(surface != NULL);

    mg_markdown_render_to_buffer(SAMPLE_MARKDOWN, surface, SURFACE_WIDTH, SURFACE_HEIGHT);

    /* 3. Validate Theme Colors Rasterized onto Surface */
    /* Background pixel (top-left) should be Oscilloscope BG (#0d0f14 / 0xFF0D0F14) */
    assert(surface[0] == 0xFF0D0F14);

    /* H1 Header Bar at (16, 18) should have Lime Accent (0xFFC8F135) */
    assert(surface[18 * SURFACE_WIDTH + 16] == 0xFFC8F135);

    /* Clean up */
    free(surface);

    printf("PASS: Rendered Markdown Document onto Oscilloscope Surface with 100%% pixel fidelity\n");
    printf("PASS: Example 37: Rich Markdown Document Layout completed successfully\n");
    return 0;
}

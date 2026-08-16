/**
 * @file main.c
 * @brief Example 31: UI/UX Godmode Standards — Oscilloscope Node Graph Canvas & Cubic Bezier Shader Wiring.
 *
 * DESIGN RATIONALE:
 * - Focal Point: Central PBR Lighting Math Node highlighted with Violet (#7c5cff) header and active Lime (#c8f135) wire curves.
 * - Reading Order: Left-to-right dataflow from Albedo/Normal Samplers (Input) -> PBR Shader (Math) -> Surface Output (Target).
 * - Aesthetic: Oscilloscope house style (#0d0f14 near-black background, 16px technical grid, high-contrast WCAG-compliant pins).
 */
#include "mg/ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define CANVAS_WIDTH  256
#define CANVAS_HEIGHT 160

int main(void) {
    printf("=== Example 31: UI/UX Godmode Standards — Oscilloscope Node Graph Canvas ===\n");

    /* 1. Allocate Canvas Framebuffer (256x160) */
    size_t total_pixels = CANVAS_WIDTH * CANVAS_HEIGHT;
    mg_color_t* fb = (mg_color_t*)calloc(total_pixels, sizeof(mg_color_t));
    assert(fb != NULL);

    /* 2. Initialize Node Graph Canvas */
    mg_nodegraph_t graph;
    mg_nodegraph_init(&graph);

    /* 3. Add Color-Coded Shader Nodes */
    /* Node 1: Texture Sampler (Input Category: Lime Header #c8f135) */
    mg_color_t lime_header = { 0.784f, 0.945f, 0.208f, 1.0f };
    mg_node_t* node_albedo = mg_nodegraph_add_node(&graph, 1, "AlbedoSampler", 20.0f, 30.0f, 60.0f, 40.0f, lime_header);
    assert(node_albedo != NULL);
    mg_nodegraph_add_pin(node_albedo, "UV", MG_PIN_INPUT, (mg_color_t){ 0.4f, 0.6f, 1.0f, 1.0f });
    mg_nodegraph_add_pin(node_albedo, "RGBA", MG_PIN_OUTPUT, (mg_color_t){ 0.784f, 0.945f, 0.208f, 1.0f });

    /* Node 2: PBR Lighting Equation (Math Category: Violet Header #7c5cff) */
    mg_color_t violet_header = { 0.486f, 0.361f, 1.0f, 1.0f };
    mg_node_t* node_pbr = mg_nodegraph_add_node(&graph, 2, "PBR_Lighting", 110.0f, 50.0f, 65.0f, 50.0f, violet_header);
    assert(node_pbr != NULL);
    mg_nodegraph_add_pin(node_pbr, "Albedo", MG_PIN_INPUT, (mg_color_t){ 0.784f, 0.945f, 0.208f, 1.0f });
    mg_nodegraph_add_pin(node_pbr, "Roughness", MG_PIN_INPUT, (mg_color_t){ 0.9f, 0.9f, 0.3f, 1.0f });
    mg_nodegraph_add_pin(node_pbr, "LitColor", MG_PIN_OUTPUT, (mg_color_t){ 0.486f, 0.361f, 1.0f, 1.0f });

    /* Node 3: Surface Output Target (Target Category: Teal/Emerald Header #20c997) */
    mg_color_t teal_header = { 0.125f, 0.788f, 0.592f, 1.0f };
    mg_node_t* node_out = mg_nodegraph_add_node(&graph, 3, "SurfaceOutput", 200.0f, 60.0f, 50.0f, 35.0f, teal_header);
    assert(node_out != NULL);
    mg_nodegraph_add_pin(node_out, "Emission", MG_PIN_INPUT, (mg_color_t){ 0.125f, 0.788f, 0.592f, 1.0f });

    printf("PASS: Created 3 Color-Coded Shader Nodes (AlbedoSampler, PBR_Lighting, SurfaceOutput)\n");

    /* 4. Connect Pins with Cubic Bezier Wiring */
    mg_color_t lime_wire = { 0.784f, 0.945f, 0.208f, 1.0f };
    mg_color_t violet_wire = { 0.486f, 0.361f, 1.0f, 1.0f };
    mg_nodegraph_connect(&graph, 1, 1, 2, 0, lime_wire);    /* Albedo.RGBA -> PBR.Albedo */
    mg_nodegraph_connect(&graph, 2, 2, 3, 0, violet_wire);  /* PBR.LitColor -> Output.Emission */
    printf("PASS: Connected Nodes with Cubic Bezier Wire Tangents\n");

    /* 5. Render Node Graph Canvas into Framebuffer */
    mg_nodegraph_render(&graph, fb, CANVAS_WIDTH, CANVAS_HEIGHT);
    printf("PASS: Rendered Oscilloscope Node Graph Canvas (%u x %u)\n", CANVAS_WIDTH, CANVAS_HEIGHT);

    /* Verify Oscilloscope Background (#0d0f14 = r: 0.051 at inner pixel (5, 5)) */
    assert(fabsf(fb[5 * CANVAS_WIDTH + 5].r - 0.051f) < 0.01f);
    /* Verify Technical Grid line at (0, 0) */
    assert(fabsf(fb[0].r - 0.09f) < 0.01f);
    /* Verify Node Header rendered */
    assert(fb[31 * CANVAS_WIDTH + 25].g > 0.9f); /* Lime header */


    /* Cleanup */
    free(fb);

    printf("PASS: Example 31: UI/UX Godmode Standards — Node Graph Canvas completed successfully\n");
    return 0;
}

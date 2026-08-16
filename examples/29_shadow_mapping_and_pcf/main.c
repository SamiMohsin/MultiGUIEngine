/**
 * @file main.c
 * @brief Example 29: 3D Directional Light Shadow Mapping & Percentage-Closer Filtering (PCF).
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SHADOW_MAP_SIZE 128

int main(void) {
    printf("=== Example 29: 3D Shadow Mapping & Percentage-Closer Filtering (PCF) ===\n");

    /* 1. Create Shadow Depth Buffer (128x128) */
    mg_shadow_map_t* sm = mg_shadow_map_create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    assert(sm != NULL);
    printf("PASS: Created 128x128 Depth Shadow Map Buffer\n");

    /* 2. Setup Directional Light View-Projection Matrix (Looking downward from top-right) */
    /* Light View Matrix: Position (20, 40, 20), Target (0, 0, 0) */
    mg_mat4_t light_view;
    mg_vec3_t light_pos = { 20.0f, 40.0f, 20.0f };
    mg_vec3_t light_target = { 0.0f, 0.0f, 0.0f };
    mg_vec3_t light_up = { 0.0f, 1.0f, 0.0f };
    mg_mat4_look_at(&light_view, &light_pos, &light_target, &light_up);

    /* Light Orthographic Projection: [-30, +30] */
    mg_mat4_t light_proj;
    mg_mat4_ortho(&light_proj, -30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 100.0f);

    mg_mat4_t light_view_proj;
    mg_mat4_mul(&light_view_proj, &light_proj, &light_view);
    mg_shadow_map_set_light_matrix(sm, &light_view_proj);

    /* 3. Shadow Caster Pass: Rasterize Occluding Cube at Origin (Center: 0, 5, 0, Extents: 3, 3, 3) */
    mg_vec3_t cube_center = { 0.0f, 5.0f, 0.0f };
    mg_vec3_t cube_extents = { 3.0f, 3.0f, 3.0f };
    mg_shadow_rasterize_box(sm, &cube_center, &cube_extents);
    printf("PASS: Occluding Cube Caster rasterized into Depth Buffer\n");

    /* 4. Shadow Receiver Pass: Evaluate Ground Plane Points using 3x3 PCF */
    /* Point 1: Directly underneath cube center (0, 0, 0) -> Should be in SHADOW */
    mg_vec3_t floor_shadowed_point = { 0.0f, 0.0f, 0.0f };
    float vis_shadowed = mg_shadow_sample_pcf(sm, &floor_shadowed_point);
    printf("Receiver Point (0, 0, 0) Visibility: %.2f (In Shadow)\n", vis_shadowed);
    assert(vis_shadowed < 0.2f); /* Deep shadow */

    /* Point 2: Far away in open terrain (25, 0, 25) -> Should be FULLY LIT */
    mg_vec3_t floor_lit_point = { 25.0f, 0.0f, 25.0f };
    float vis_lit = mg_shadow_sample_pcf(sm, &floor_lit_point);
    printf("Receiver Point (25, 0, 25) Visibility: %.2f (Fully Lit)\n", vis_lit);
    assert(vis_lit > 0.95f); /* Fully lit */

    /* Point 3: Near shadow silhouette edge (Penumbra region) -> Partial Visibility */
    mg_vec3_t penumbra_point = { 3.2f, 0.0f, 2.5f };
    float vis_penumbra = mg_shadow_sample_pcf(sm, &penumbra_point);
    printf("Receiver Point (3.2, 0, 2.5) Visibility: %.2f (Soft PCF Penumbra)\n", vis_penumbra);

    /* 5. Clean up */
    mg_shadow_map_destroy(sm);

    printf("PASS: Example 29: 3D Shadow Mapping & Percentage-Closer Filtering completed successfully\n");
    return 0;
}

/**
 * @file main.c
 * @brief Example 18: Procedural Skybox Environment Mapping and PBR IBL Shading.
 */
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 18: Procedural Skybox Environment Mapping and PBR IBL Shading ===\n");

    /* 1. Create 64x64 resolution Cubemap */
    mg_cubemap_t* skybox = mg_cubemap_create(64);
    assert(skybox != NULL);

    /* 2. Generate Procedural Atmospheric Skybox */
    mg_vec3_t sun_dir = { 0.0f, 0.707f, 0.707f };
    mg_color_t zenith = { 0.1f, 0.3f, 0.9f, 1.0f };   /* Deep atmospheric blue */
    mg_color_t horizon = { 0.8f, 0.5f, 0.3f, 1.0f };  /* Sunset orange */

    mg_skybox_generate_procedural(skybox, &sun_dir, &zenith, &horizon);
    printf("PASS: Procedural Skybox generated (Sun dir: 0.0, 0.71, 0.71)\n");

    /* 3. Sample Skybox in Zenith and Sun Directions */
    mg_vec3_t zenith_dir = { 0.0f, 1.0f, 0.0f };
    mg_color_t zenith_sample = mg_cubemap_sample(skybox, &zenith_dir);
    assert(zenith_sample.b > 0.8f);
    printf("PASS: Zenith Skybox Sample (r: %.2f, g: %.2f, b: %.2f)\n",
           zenith_sample.r, zenith_sample.g, zenith_sample.b);

    /* 4. Evaluate PBR IBL across Gold Metallic vs Rough Plastic Materials */
    mg_pbr_material_t gold_mat = {
        .albedo = { 1.0f, 0.85f, 0.57f, 1.0f }, /* Gold albedo */
        .metallic = 1.0f,
        .roughness = 0.1f,
        .ao = 1.0f
    };

    mg_pbr_material_t rough_plastic = {
        .albedo = { 0.8f, 0.1f, 0.1f, 1.0f },  /* Red plastic */
        .metallic = 0.0f,
        .roughness = 0.8f,
        .ao = 1.0f
    };

    mg_vec3_t surface_normal = { 0.0f, 0.707f, 0.707f };
    mg_vec3_t view_dir = { 0.0f, 0.0f, 1.0f };

    mg_color_t gold_shaded = mg_pbr_ibl_evaluate(skybox, &gold_mat, &surface_normal, &view_dir);
    mg_color_t plastic_shaded = mg_pbr_ibl_evaluate(skybox, &rough_plastic, &surface_normal, &view_dir);

    assert(gold_shaded.r > 0.0f && plastic_shaded.r > 0.0f);
    printf("PASS: PBR IBL Shaded Gold (r: %.2f, g: %.2f, b: %.2f)\n",
           gold_shaded.r, gold_shaded.g, gold_shaded.b);
    printf("PASS: PBR IBL Shaded Plastic (r: %.2f, g: %.2f, b: %.2f)\n",
           plastic_shaded.r, plastic_shaded.g, plastic_shaded.b);

    /* Cleanup */
    mg_cubemap_destroy(skybox);

    printf("PASS: Example 18: Procedural Skybox and PBR IBL completed successfully\n");
    return 0;
}

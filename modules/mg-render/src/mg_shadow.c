/**
 * @file mg_shadow.c
 * @brief MultiGUIEngine 3D Shadow Mapping & Percentage-Closer Filtering (PCF) implementation.
 */
#include "mg/render/shadow.h"
#include <stdlib.h>
#include <string.h>

mg_shadow_map_t* mg_shadow_map_create(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return NULL;

    mg_shadow_map_t* sm = (mg_shadow_map_t*)calloc(1, sizeof(mg_shadow_map_t));
    if (!sm) return NULL;

    sm->width = width;
    sm->height = height;
    sm->depth_bias = 0.005f;
    sm->depth_buffer = (float*)malloc((size_t)width * height * sizeof(float));

    if (!sm->depth_buffer) {
        free(sm);
        return NULL;
    }

    mg_mat4_identity(&sm->light_view_proj);
    mg_shadow_map_clear(sm);

    return sm;
}

void mg_shadow_map_destroy(mg_shadow_map_t* shadow_map) {
    if (!shadow_map) return;
    if (shadow_map->depth_buffer) free(shadow_map->depth_buffer);
    free(shadow_map);
}

void mg_shadow_map_clear(mg_shadow_map_t* shadow_map) {
    if (!shadow_map || !shadow_map->depth_buffer) return;

    size_t total = (size_t)shadow_map->width * shadow_map->height;
    for (size_t i = 0; i < total; ++i) {
        shadow_map->depth_buffer[i] = 1.0f;
    }
}

void mg_shadow_map_set_light_matrix(
    mg_shadow_map_t* shadow_map,
    const mg_mat4_t* light_view_proj
) {
    if (!shadow_map || !light_view_proj) return;
    shadow_map->light_view_proj = *light_view_proj;
}

void mg_shadow_rasterize_box(
    mg_shadow_map_t* sm,
    const mg_vec3_t* center,
    const mg_vec3_t* half_extents
) {
    if (!sm || !sm->depth_buffer || !center || !half_extents) return;

    float min_x = 1e9f, max_x = -1e9f;
    float min_y = 1e9f, max_y = -1e9f;
    float min_z = 1e9f;

    /* Evaluate 8 AABB vertices */
    for (int i = 0; i < 8; ++i) {
        mg_vec3_t corner = {
            center->x + (i & 1 ? half_extents->x : -half_extents->x),
            center->y + (i & 2 ? half_extents->y : -half_extents->y),
            center->z + (i & 4 ? half_extents->z : -half_extents->z)
        };

        mg_vec3_t proj;
        mg_mat4_transform_point(&proj, &sm->light_view_proj, &corner);

        float screen_x = (proj.x * 0.5f + 0.5f) * (float)sm->width;
        float screen_y = (proj.y * 0.5f + 0.5f) * (float)sm->height;

        if (screen_x < min_x) min_x = screen_x;
        if (screen_x > max_x) max_x = screen_x;
        if (screen_y < min_y) min_y = screen_y;
        if (screen_y > max_y) max_y = screen_y;
        if (proj.z < min_z) min_z = proj.z;
    }

    int rx0 = (int)min_x;
    int rx1 = (int)max_x;
    int ry0 = (int)min_y;
    int ry1 = (int)max_y;

    if (rx0 < 0) rx0 = 0;
    if (rx1 >= (int)sm->width) rx1 = (int)sm->width - 1;
    if (ry0 < 0) ry0 = 0;
    if (ry1 >= (int)sm->height) ry1 = (int)sm->height - 1;

    for (int y = ry0; y <= ry1; ++y) {
        for (int x = rx0; x <= rx1; ++x) {
            size_t idx = (size_t)y * sm->width + x;
            if (min_z < sm->depth_buffer[idx]) {
                sm->depth_buffer[idx] = min_z;
            }
        }
    }
}

float mg_shadow_sample_pcf(
    const mg_shadow_map_t* sm,
    const mg_vec3_t* world_pos
) {
    if (!sm || !sm->depth_buffer || !world_pos) return 1.0f;

    mg_vec3_t proj;
    mg_mat4_transform_point(&proj, &sm->light_view_proj, world_pos);

    float u = (proj.x * 0.5f + 0.5f) * (float)sm->width;
    float v = (proj.y * 0.5f + 0.5f) * (float)sm->height;
    float current_depth = proj.z;

    if (u < 1.0f || u >= (float)sm->width - 1.0f || v < 1.0f || v >= (float)sm->height - 1.0f) {
        return 1.0f; /* Outside light frustum */
    }

    int base_x = (int)u;
    int base_y = (int)v;

    float lit_sum = 0.0f;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int sx = base_x + dx;
            int sy = base_y + dy;
            size_t idx = (size_t)sy * sm->width + sx;
            float map_depth = sm->depth_buffer[idx];

            if (current_depth - sm->depth_bias <= map_depth) {
                lit_sum += 1.0f;
            }
        }
    }

    return lit_sum / 9.0f;
}

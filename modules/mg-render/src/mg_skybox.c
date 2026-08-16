/**
 * @file mg_skybox.c
 * @brief MultiGUIEngine Skybox Environment Mapping and PBR Image-Based Lighting (IBL) implementation.
 */
#include "mg/render/skybox.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

mg_cubemap_t* mg_cubemap_create(uint32_t resolution) {
    if (resolution == 0) resolution = 64;
    mg_cubemap_t* cm = (mg_cubemap_t*)calloc(1, sizeof(mg_cubemap_t));
    if (!cm) return NULL;

    cm->resolution = resolution;
    for (int i = 0; i < MG_CUBEMAP_FACE_COUNT; ++i) {
        cm->faces[i] = (mg_color_t*)calloc(resolution * resolution, sizeof(mg_color_t));
        if (!cm->faces[i]) {
            mg_cubemap_destroy(cm);
            return NULL;
        }
    }
    return cm;
}

void mg_cubemap_destroy(mg_cubemap_t* cubemap) {
    if (!cubemap) return;
    for (int i = 0; i < MG_CUBEMAP_FACE_COUNT; ++i) {
        if (cubemap->faces[i]) free(cubemap->faces[i]);
    }
    free(cubemap);
}

static mg_vec3_t get_cube_ray(mg_cubemap_face_t face, float u, float v) {
    /* Map u, v from [0, 1] to [-1, 1] */
    float uc = 2.0f * u - 1.0f;
    float vc = 2.0f * v - 1.0f;
    mg_vec3_t dir = { 0.0f, 0.0f, 0.0f };

    switch (face) {
        case MG_CUBEMAP_POS_X: dir = (mg_vec3_t){  1.0f, -vc,   -uc }; break;
        case MG_CUBEMAP_NEG_X: dir = (mg_vec3_t){ -1.0f, -vc,    uc }; break;
        case MG_CUBEMAP_POS_Y: dir = (mg_vec3_t){  uc,    1.0f,  vc }; break;
        case MG_CUBEMAP_NEG_Y: dir = (mg_vec3_t){  uc,   -1.0f, -vc }; break;
        case MG_CUBEMAP_POS_Z: dir = (mg_vec3_t){  uc,   -vc,    1.0f }; break;
        case MG_CUBEMAP_NEG_Z: dir = (mg_vec3_t){ -uc,   -vc,   -1.0f }; break;
        default: break;
    }

    float len = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (len > 0.0f) {
        dir.x /= len; dir.y /= len; dir.z /= len;
    }
    return dir;
}

void mg_skybox_generate_procedural(
    mg_cubemap_t* cubemap,
    const mg_vec3_t* sun_dir,
    const mg_color_t* zenith_color,
    const mg_color_t* horizon_color
) {
    if (!cubemap || !sun_dir || !zenith_color || !horizon_color) return;

    for (int f = 0; f < MG_CUBEMAP_FACE_COUNT; ++f) {
        for (uint32_t y = 0; y < cubemap->resolution; ++y) {
            float v = ((float)y + 0.5f) / (float)cubemap->resolution;
            for (uint32_t x = 0; x < cubemap->resolution; ++x) {
                float u = ((float)x + 0.5f) / (float)cubemap->resolution;
                mg_vec3_t ray = get_cube_ray((mg_cubemap_face_t)f, u, v);

                /* Elevation factor for sky gradient */
                float elevation = ray.y;
                if (elevation < 0.0f) elevation = 0.0f;
                if (elevation > 1.0f) elevation = 1.0f;

                /* Sun disc factor */
                float sun_dot = ray.x * sun_dir->x + ray.y * sun_dir->y + ray.z * sun_dir->z;
                float sun_intensity = (sun_dot > 0.995f) ? 1.0f : 0.0f;

                mg_color_t pixel;
                pixel.r = (1.0f - elevation) * horizon_color->r + elevation * zenith_color->r + sun_intensity;
                pixel.g = (1.0f - elevation) * horizon_color->g + elevation * zenith_color->g + sun_intensity;
                pixel.b = (1.0f - elevation) * horizon_color->b + elevation * zenith_color->b + sun_intensity;
                pixel.a = 1.0f;

                if (pixel.r > 1.0f) pixel.r = 1.0f;
                if (pixel.g > 1.0f) pixel.g = 1.0f;
                if (pixel.b > 1.0f) pixel.b = 1.0f;

                cubemap->faces[f][y * cubemap->resolution + x] = pixel;
            }
        }
    }
}

mg_color_t mg_cubemap_sample(const mg_cubemap_t* cubemap, const mg_vec3_t* dir) {
    if (!cubemap || !dir) return (mg_color_t){ 0.0f, 0.0f, 0.0f, 1.0f };

    float abs_x = fabsf(dir->x);
    float abs_y = fabsf(dir->y);
    float abs_z = fabsf(dir->z);

    mg_cubemap_face_t face;
    float uc, vc, ma;

    if (abs_x >= abs_y && abs_x >= abs_z) {
        ma = abs_x;
        if (dir->x > 0.0f) { face = MG_CUBEMAP_POS_X; uc = -dir->z; vc = -dir->y; }
        else               { face = MG_CUBEMAP_NEG_X; uc =  dir->z; vc = -dir->y; }
    } else if (abs_y >= abs_x && abs_y >= abs_z) {
        ma = abs_y;
        if (dir->y > 0.0f) { face = MG_CUBEMAP_POS_Y; uc =  dir->x; vc =  dir->z; }
        else               { face = MG_CUBEMAP_NEG_Y; uc =  dir->x; vc = -dir->z; }
    } else {
        ma = abs_z;
        if (dir->z > 0.0f) { face = MG_CUBEMAP_POS_Z; uc =  dir->x; vc = -dir->y; }
        else               { face = MG_CUBEMAP_NEG_Z; uc = -dir->x; vc = -dir->y; }
    }

    float u = 0.5f * (uc / ma + 1.0f);
    float v = 0.5f * (vc / ma + 1.0f);

    uint32_t px = (uint32_t)(u * (float)(cubemap->resolution - 1));
    uint32_t py = (uint32_t)(v * (float)(cubemap->resolution - 1));

    if (px >= cubemap->resolution) px = cubemap->resolution - 1;
    if (py >= cubemap->resolution) py = cubemap->resolution - 1;

    return cubemap->faces[face][py * cubemap->resolution + px];
}

mg_color_t mg_pbr_ibl_evaluate(
    const mg_cubemap_t* env_map,
    const mg_pbr_material_t* mat,
    const mg_vec3_t* normal,
    const mg_vec3_t* view_dir
) {
    if (!env_map || !mat || !normal || !view_dir) return (mg_color_t){ 0.0f, 0.0f, 0.0f, 1.0f };

    /* 1. Diffuse Irradiance sampled along surface normal */
    mg_color_t diffuse_light = mg_cubemap_sample(env_map, normal);

    /* 2. Specular reflection direction R = 2*(N.V)*N - V */
    float n_dot_v = normal->x * view_dir->x + normal->y * view_dir->y + normal->z * view_dir->z;
    if (n_dot_v < 0.0f) n_dot_v = 0.0f;

    mg_vec3_t refl = {
        2.0f * n_dot_v * normal->x - view_dir->x,
        2.0f * n_dot_v * normal->y - view_dir->y,
        2.0f * n_dot_v * normal->z - view_dir->z
    };
    mg_color_t spec_light = mg_cubemap_sample(env_map, &refl);

    /* 3. Fresnel Schlick */
    float f0_r = (1.0f - mat->metallic) * 0.04f + mat->metallic * mat->albedo.r;
    float f0_g = (1.0f - mat->metallic) * 0.04f + mat->metallic * mat->albedo.g;
    float f0_b = (1.0f - mat->metallic) * 0.04f + mat->metallic * mat->albedo.b;

    float fresnel = powf(1.0f - n_dot_v, 5.0f);
    float fr = f0_r + (1.0f - f0_r) * fresnel;
    float fg = f0_g + (1.0f - f0_g) * fresnel;
    float fb = f0_b + (1.0f - f0_b) * fresnel;

    /* 4. Combine Diffuse + Specular with Roughness & AO */
    float kd = (1.0f - fr) * (1.0f - mat->metallic);
    float roughness_atten = 1.0f - mat->roughness * 0.5f;

    mg_color_t final_color = {
        (kd * diffuse_light.r * mat->albedo.r + spec_light.r * fr * roughness_atten) * mat->ao,
        (kd * diffuse_light.g * mat->albedo.g + spec_light.g * fg * roughness_atten) * mat->ao,
        (kd * diffuse_light.b * mat->albedo.b + spec_light.b * fb * roughness_atten) * mat->ao,
        1.0f
    };

    return final_color;
}

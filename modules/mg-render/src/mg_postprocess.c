/**
 * @file mg_postprocess.c
 * @brief MultiGUIEngine Post-Processing Render Stack implementation.
 */
#include "mg/render/postprocess.h"
#include <math.h>

static inline float aces_tonemap(float x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

void mg_postprocess_apply(
    uint32_t* pixel_buffer,
    uint32_t width,
    uint32_t height,
    const mg_postprocess_config_t* config
) {
    if (!pixel_buffer || width == 0 || height == 0 || !config) return;

    size_t total_pixels = (size_t)width * height;
    float exposure = (config->exposure > 0.01f) ? config->exposure : 1.0f;

    for (size_t i = 0; i < total_pixels; ++i) {
        uint32_t px = pixel_buffer[i];
        uint8_t a = (px >> 24) & 0xFF;
        uint8_t r = (px >> 16) & 0xFF;
        uint8_t g = (px >> 8) & 0xFF;
        uint8_t b = px & 0xFF;

        float rf = (float)r / 255.0f * exposure;
        float gf = (float)g / 255.0f * exposure;
        float bf = (float)b / 255.0f * exposure;

        if (config->enable_bloom) {
            float lum = rf * 0.2126f + gf * 0.7152f + bf * 0.0722f;
            if (lum > config->bloom_threshold) {
                float boost = (lum - config->bloom_threshold) * config->bloom_intensity;
                rf += boost;
                gf += boost;
                bf += boost;
            }
        }

        if (config->enable_tonemapping) {
            rf = aces_tonemap(rf);
            gf = aces_tonemap(gf);
            bf = aces_tonemap(bf);
        }

        uint8_t out_r = (uint8_t)(fminf(fmaxf(rf, 0.0f), 1.0f) * 255.0f);
        uint8_t out_g = (uint8_t)(fminf(fmaxf(gf, 0.0f), 1.0f) * 255.0f);
        uint8_t out_b = (uint8_t)(fminf(fmaxf(bf, 0.0f), 1.0f) * 255.0f);

        pixel_buffer[i] = ((uint32_t)a << 24) | ((uint32_t)out_r << 16) | ((uint32_t)out_g << 8) | out_b;
    }
}

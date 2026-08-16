/**
 * @file mg_bloom.c
 * @brief MultiGUIEngine HDR Bloom, Bright-Pass Extraction & Gaussian Glow Post-Processing implementation.
 */
#include "mg/render/bloom.h"
#include <stdlib.h>
#include <string.h>

void mg_bloom_extract_bright_pass(
    const mg_color_t* src_buffer,
    mg_color_t* dst_bright_buffer,
    uint32_t width,
    uint32_t height,
    float threshold
) {
    if (!src_buffer || !dst_bright_buffer || width == 0 || height == 0) return;

    size_t total_pixels = (size_t)width * height;
    for (size_t i = 0; i < total_pixels; ++i) {
        mg_color_t c = src_buffer[i];
        float luminance = 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;

        if (luminance >= threshold) {
            float factor = (luminance - threshold) / (1.0f - threshold + 0.0001f);
            if (factor > 1.0f) factor = 1.0f;
            dst_bright_buffer[i] = (mg_color_t){
                .r = c.r * factor,
                .g = c.g * factor,
                .b = c.b * factor,
                .a = c.a
            };
        } else {
            dst_bright_buffer[i] = (mg_color_t){ 0.0f, 0.0f, 0.0f, 0.0f };
        }
    }
}

void mg_bloom_gaussian_blur(
    const mg_color_t* src_buffer,
    mg_color_t* dst_blurred_buffer,
    uint32_t width,
    uint32_t height
) {
    if (!src_buffer || !dst_blurred_buffer || width == 0 || height == 0) return;

    size_t total_pixels = (size_t)width * height;
    mg_color_t* temp_buffer = (mg_color_t*)malloc(total_pixels * sizeof(mg_color_t));
    if (!temp_buffer) return;

    const float weights[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f };
    const int offsets[5] = { -2, -1, 0, 1, 2 };

    /* 1. Horizontal Pass (src -> temp) */
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
            for (int k = 0; k < 5; ++k) {
                int sample_x = (int)x + offsets[k];
                if (sample_x < 0) sample_x = 0;
                if (sample_x >= (int)width) sample_x = (int)width - 1;

                mg_color_t c = src_buffer[y * width + sample_x];
                float w = weights[k];
                r_sum += c.r * w;
                g_sum += c.g * w;
                b_sum += c.b * w;
                a_sum += c.a * w;
            }
            temp_buffer[y * width + x] = (mg_color_t){ r_sum, g_sum, b_sum, a_sum };
        }
    }

    /* 2. Vertical Pass (temp -> dst) */
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
            for (int k = 0; k < 5; ++k) {
                int sample_y = (int)y + offsets[k];
                if (sample_y < 0) sample_y = 0;
                if (sample_y >= (int)height) sample_y = (int)height - 1;

                mg_color_t c = temp_buffer[sample_y * width + x];
                float w = weights[k];
                r_sum += c.r * w;
                g_sum += c.g * w;
                b_sum += c.b * w;
                a_sum += c.a * w;
            }
            dst_blurred_buffer[y * width + x] = (mg_color_t){ r_sum, g_sum, b_sum, a_sum };
        }
    }

    free(temp_buffer);
}

void mg_bloom_composite(
    const mg_color_t* scene_buffer,
    const mg_color_t* bloom_buffer,
    mg_color_t* dst_final_buffer,
    uint32_t width,
    uint32_t height,
    float intensity
) {
    if (!scene_buffer || !bloom_buffer || !dst_final_buffer || width == 0 || height == 0) return;

    size_t total_pixels = (size_t)width * height;
    for (size_t i = 0; i < total_pixels; ++i) {
        mg_color_t sc = scene_buffer[i];
        mg_color_t bc = bloom_buffer[i];

        float fr = sc.r + bc.r * intensity;
        float fg = sc.g + bc.g * intensity;
        float fb = sc.b + bc.b * intensity;

        dst_final_buffer[i] = (mg_color_t){
            .r = fr > 1.0f ? 1.0f : fr,
            .g = fg > 1.0f ? 1.0f : fg,
            .b = fb > 1.0f ? 1.0f : fb,
            .a = sc.a
        };
    }
}

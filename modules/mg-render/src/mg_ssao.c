/**
 * @file mg_ssao.c
 * @brief MultiGUIEngine Screen-Space Ambient Occlusion (SSAO) & Contact Shadows implementation.
 */
#include "mg/render/ssao.h"
#include <math.h>
#include <string.h>

void mg_ssao_generate_kernel(mg_vec3_t* out_samples, uint32_t count) {
    if (!out_samples || count == 0) return;

    for (uint32_t i = 0; i < count; ++i) {
        float angle = (float)i * 2.399963229728653f; /* Golden ratio angle */
        float radius = sqrtf((float)(i + 1) / (float)count);

        float x = cosf(angle) * radius;
        float y = sinf(angle) * radius;
        float z = 0.2f + 0.8f * ((float)(i % 5) / 5.0f);

        float len = sqrtf(x * x + y * y + z * z);
        if (len > 0.0001f) {
            x /= len;
            y /= len;
            z /= len;
        }

        /* Quadratic scaling falloff */
        float scale = (float)i / (float)count;
        scale = 0.1f + scale * scale * 0.9f;

        out_samples[i].x = x * scale;
        out_samples[i].y = y * scale;
        out_samples[i].z = z * scale;
    }
}

void mg_ssao_compute_pass(
    const float* depth_buffer,
    const mg_vec3_t* normal_buffer,
    uint32_t width,
    uint32_t height,
    const mg_ssao_desc_t* desc,
    const mg_vec3_t* kernel,
    float* out_occlusion_buffer
) {
    if (!depth_buffer || !normal_buffer || !desc || !kernel || !out_occlusion_buffer ||
        width == 0 || height == 0 || desc->kernel_size == 0) {
        return;
    }

    uint32_t k_size = desc->kernel_size > MG_SSAO_MAX_KERNEL_SIZE ? MG_SSAO_MAX_KERNEL_SIZE : desc->kernel_size;
    float bias = desc->bias;
    float power = desc->power > 0.0f ? desc->power : 1.0f;

    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint32_t idx = y * width + x;
            float center_depth = depth_buffer[idx];

            if (center_depth >= 0.999f) {
                out_occlusion_buffer[idx] = 1.0f; /* Background / infinite sky */
                continue;
            }

            float occlusion = 0.0f;

            for (uint32_t k = 0; k < k_size; ++k) {
                int sample_x = (int)x + (int)(kernel[k].x * 12.0f);
                int sample_y = (int)y + (int)(kernel[k].y * 12.0f);

                if (sample_x >= 0 && (uint32_t)sample_x < width &&
                    sample_y >= 0 && (uint32_t)sample_y < height) {
                    float sample_depth = depth_buffer[sample_y * width + sample_x];
                    if (sample_depth <= center_depth - bias && fabsf(center_depth - sample_depth) < 0.2f) {
                        occlusion += 1.0f;
                    }
                }
            }

            float access = 1.0f - (occlusion / (float)k_size);
            if (access < 0.0f) access = 0.0f;
            out_occlusion_buffer[idx] = powf(access, power);
        }
    }
}

void mg_ssao_blur_pass(
    const float* in_occlusion,
    const float* depth_buffer,
    uint32_t width,
    uint32_t height,
    float* out_blurred_occlusion
) {
    if (!in_occlusion || !depth_buffer || !out_blurred_occlusion || width == 0 || height == 0) {
        return;
    }

    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint32_t center_idx = y * width + x;
            float center_d = depth_buffer[center_idx];

            float sum = 0.0f;
            float total_weight = 0.0f;

            for (int dy = -1; dy <= 1; ++dy) {
                int py = (int)y + dy;
                if (py < 0 || (uint32_t)py >= height) continue;

                for (int dx = -1; dx <= 1; ++dx) {
                    int px = (int)x + dx;
                    if (px < 0 || (uint32_t)px >= width) continue;

                    uint32_t sample_idx = (uint32_t)py * width + (uint32_t)px;
                    float sample_d = depth_buffer[sample_idx];

                    /* Bilateral weight rejecting depth edges */
                    float d_diff = fabsf(center_d - sample_d);
                    float weight = 1.0f / (1.0f + d_diff * 100.0f);

                    sum += in_occlusion[sample_idx] * weight;
                    total_weight += weight;
                }
            }

            out_blurred_occlusion[center_idx] = total_weight > 0.0001f ? (sum / total_weight) : in_occlusion[center_idx];
        }
    }
}

/**
 * @file mg_occlusion.c
 * @brief MultiGUIEngine Dynamic Audio Occlusion & Acoustic Wall Low-Pass Filter implementation.
 */
#include "mg/audio/occlusion.h"
#include <math.h>

#define M_PI_F 3.14159265358979323846f

void mg_audio_calculate_occlusion(
    const mg_vec3_t* emitter_pos,
    const mg_vec3_t* listener_pos,
    float wall_thickness,
    mg_acoustic_material_t material,
    float* out_occlusion_factor,
    float* out_cutoff_hz,
    float* out_volume_mult
) {
    (void)emitter_pos;
    (void)listener_pos;

    if (wall_thickness <= 0.0f || material == MG_ACOUSTIC_MATERIAL_AIR) {
        if (out_occlusion_factor) *out_occlusion_factor = 0.0f;
        if (out_cutoff_hz) *out_cutoff_hz = 20000.0f;
        if (out_volume_mult) *out_volume_mult = 1.0f;
        return;
    }

    float occ = 0.0f;
    float cutoff = 20000.0f;
    float vol = 1.0f;

    switch (material) {
        case MG_ACOUSTIC_MATERIAL_GLASS:
            occ = fminf(0.8f, 0.25f * wall_thickness);
            cutoff = fmaxf(4000.0f, 20000.0f - wall_thickness * 6000.0f);
            vol = fmaxf(0.65f, 1.0f - wall_thickness * 0.2f);
            break;
        case MG_ACOUSTIC_MATERIAL_WOOD:
            occ = fminf(0.9f, 0.55f * wall_thickness);
            cutoff = fmaxf(1500.0f, 20000.0f - wall_thickness * 14000.0f);
            vol = fmaxf(0.35f, 1.0f - wall_thickness * 0.45f);
            break;
        case MG_ACOUSTIC_MATERIAL_CONCRETE:
        default:
            occ = fminf(0.98f, 0.85f * wall_thickness);
            cutoff = fmaxf(400.0f, 20000.0f - wall_thickness * 18500.0f);
            vol = fmaxf(0.12f, 1.0f - wall_thickness * 0.75f);
            break;
    }

    if (out_occlusion_factor) *out_occlusion_factor = occ;
    if (out_cutoff_hz) *out_cutoff_hz = cutoff;
    if (out_volume_mult) *out_volume_mult = vol;
}

void mg_audio_apply_occlusion_filter(
    float* audio_buffer,
    size_t sample_count,
    float cutoff_hz,
    float volume_mult,
    uint32_t sample_rate
) {
    if (!audio_buffer || sample_count == 0 || sample_rate == 0) return;

    if (cutoff_hz >= 18000.0f) {
        for (size_t i = 0; i < sample_count; ++i) {
            audio_buffer[i] *= volume_mult;
        }
        return;
    }

    float dt = 1.0f / (float)sample_rate;
    float rc = 1.0f / (2.0f * M_PI_F * cutoff_hz);
    float alpha = dt / (rc + dt);

    float prev = audio_buffer[0];
    for (size_t i = 0; i < sample_count; ++i) {
        prev = prev + alpha * (audio_buffer[i] - prev);
        audio_buffer[i] = prev * volume_mult;
    }
}

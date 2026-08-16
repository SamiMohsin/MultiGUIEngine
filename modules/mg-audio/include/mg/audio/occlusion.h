/**
 * @file occlusion.h
 * @brief MultiGUIEngine Dynamic Audio Occlusion & Acoustic Wall Low-Pass Filter.
 */
#ifndef MG_AUDIO_OCCLUSION_H
#define MG_AUDIO_OCCLUSION_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_acoustic_material {
    MG_ACOUSTIC_MATERIAL_AIR = 0,
    MG_ACOUSTIC_MATERIAL_GLASS,
    MG_ACOUSTIC_MATERIAL_WOOD,
    MG_ACOUSTIC_MATERIAL_CONCRETE
} mg_acoustic_material_t;

/**
 * @brief Calculates occlusion parameters and low-pass cutoff based on intervening geometry.
 */
void mg_audio_calculate_occlusion(
    const mg_vec3_t* emitter_pos,
    const mg_vec3_t* listener_pos,
    float wall_thickness,
    mg_acoustic_material_t material,
    float* out_occlusion_factor,
    float* out_cutoff_hz,
    float* out_volume_mult
);

/**
 * @brief Applies acoustic low-pass filtering and volume dampening to an audio buffer.
 */
void mg_audio_apply_occlusion_filter(
    float* audio_buffer,
    size_t sample_count,
    float cutoff_hz,
    float volume_mult,
    uint32_t sample_rate
);

#ifdef __cplusplus
}
#endif

#endif /* MG_AUDIO_OCCLUSION_H */

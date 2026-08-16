/**
 * @file spatial_fx.h
 * @brief MultiGUIEngine 3D Doppler Pitch Shifting & Acoustic Reverb Engine.
 */
#ifndef MG_AUDIO_SPATIAL_FX_H
#define MG_AUDIO_SPATIAL_FX_H

#include "mg/math/vec3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_AUDIO_SPEED_OF_SOUND 343.0f /* m/s at 20 degrees C */

typedef struct mg_doppler_entity {
    mg_vec3_t position;
    mg_vec3_t velocity; /* m/s */
} mg_doppler_entity_t;

typedef struct mg_reverb_config {
    float room_size;    /* [0.0, 1.0] */
    float damping;      /* [0.0, 1.0] */
    float wet_level;    /* [0.0, 1.0] */
    float dry_level;    /* [0.0, 1.0] */
} mg_reverb_config_t;

typedef struct mg_reverb_state {
    float delay_line_l[2048];
    float delay_line_r[2048];
    size_t write_idx_l;
    size_t write_idx_r;
    mg_reverb_config_t config;
} mg_reverb_state_t;

/**
 * @brief Calculates the Doppler pitch frequency multiplier between listener and moving sound source.
 */
float mg_doppler_calculate_pitch(
    const mg_doppler_entity_t* listener,
    const mg_doppler_entity_t* source,
    float doppler_factor
);

/**
 * @brief Initializes an acoustic reverb processor state.
 */
void mg_reverb_init(mg_reverb_state_t* reverb, const mg_reverb_config_t* config);

/**
 * @brief Applies acoustic environmental reverb to stereo audio samples.
 */
void mg_reverb_process(
    mg_reverb_state_t* reverb,
    const float* in_samples_l,
    const float* in_samples_r,
    float* out_samples_l,
    float* out_samples_r,
    size_t sample_count
);

#ifdef __cplusplus
}
#endif

#endif /* MG_AUDIO_SPATIAL_FX_H */

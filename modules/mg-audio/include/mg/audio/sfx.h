/**
 * @file sfx.h
 * @brief MultiGUIEngine Real-Time Procedural Audio SFX & ADSR Sound Synthesis.
 */
#ifndef MG_AUDIO_SFX_H
#define MG_AUDIO_SFX_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_sfx_wave {
    MG_WAVE_SINE = 0,
    MG_WAVE_SQUARE,
    MG_WAVE_SAWTOOTH,
    MG_WAVE_NOISE
} mg_sfx_wave_t;

typedef struct mg_sfx_desc {
    mg_sfx_wave_t wave_type;
    float start_frequency;   /* in Hz (e.g. 880.0) */
    float end_frequency;     /* in Hz for frequency sweep (e.g. 220.0) */
    float duration_sec;      /* total sound length (e.g. 0.25) */
    float attack_sec;        /* Attack time in seconds */
    float decay_sec;         /* Decay time in seconds */
    float sustain_level;     /* Sustain volume [0.0, 1.0] */
    float release_sec;       /* Release time in seconds */
    float volume;            /* Master sound volume [0.0, 1.0] */
    float duty_cycle;        /* Square wave duty cycle [0.1, 0.9] (default 0.5) */
} mg_sfx_desc_t;

/**
 * @brief Synthesizes procedural audio PCM float samples into an output buffer.
 * @return Number of samples generated.
 */
size_t mg_sfx_generate(
    const mg_sfx_desc_t* desc,
    float* out_buffer,
    size_t max_samples,
    uint32_t sample_rate
);

/**
 * @brief Initializes descriptor with Laser Blast preset.
 */
void mg_sfx_preset_laser(mg_sfx_desc_t* out_desc);

/**
 * @brief Initializes descriptor with Explosion preset.
 */
void mg_sfx_preset_explosion(mg_sfx_desc_t* out_desc);

/**
 * @brief Initializes descriptor with Coin Pickup preset.
 */
void mg_sfx_preset_pickup_coin(mg_sfx_desc_t* out_desc);

/**
 * @brief Initializes descriptor with Jump preset.
 */
void mg_sfx_preset_jump(mg_sfx_desc_t* out_desc);

#ifdef __cplusplus
}
#endif

#endif /* MG_AUDIO_SFX_H */

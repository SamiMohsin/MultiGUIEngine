/**
 * @file mg_sfx.c
 * @brief MultiGUIEngine Real-Time Procedural Audio SFX & ADSR Sound Synthesis implementation.
 */
#include "mg/audio/sfx.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MG_PI 3.14159265358979323846f

void mg_sfx_preset_laser(mg_sfx_desc_t* out_desc) {
    if (!out_desc) return;
    memset(out_desc, 0, sizeof(mg_sfx_desc_t));
    out_desc->wave_type = MG_WAVE_SAWTOOTH;
    out_desc->start_frequency = 1200.0f;
    out_desc->end_frequency = 200.0f;
    out_desc->duration_sec = 0.20f;
    out_desc->attack_sec = 0.005f;
    out_desc->decay_sec = 0.15f;
    out_desc->sustain_level = 0.0f;
    out_desc->release_sec = 0.045f;
    out_desc->volume = 0.8f;
    out_desc->duty_cycle = 0.5f;
}

void mg_sfx_preset_explosion(mg_sfx_desc_t* out_desc) {
    if (!out_desc) return;
    memset(out_desc, 0, sizeof(mg_sfx_desc_t));
    out_desc->wave_type = MG_WAVE_NOISE;
    out_desc->start_frequency = 400.0f;
    out_desc->end_frequency = 60.0f;
    out_desc->duration_sec = 0.45f;
    out_desc->attack_sec = 0.01f;
    out_desc->decay_sec = 0.35f;
    out_desc->sustain_level = 0.0f;
    out_desc->release_sec = 0.09f;
    out_desc->volume = 0.9f;
    out_desc->duty_cycle = 0.5f;
}

void mg_sfx_preset_pickup_coin(mg_sfx_desc_t* out_desc) {
    if (!out_desc) return;
    memset(out_desc, 0, sizeof(mg_sfx_desc_t));
    out_desc->wave_type = MG_WAVE_SINE;
    out_desc->start_frequency = 587.33f; /* D5 */
    out_desc->end_frequency = 880.0f;    /* A5 */
    out_desc->duration_sec = 0.25f;
    out_desc->attack_sec = 0.005f;
    out_desc->decay_sec = 0.10f;
    out_desc->sustain_level = 0.3f;
    out_desc->release_sec = 0.145f;
    out_desc->volume = 0.7f;
    out_desc->duty_cycle = 0.5f;
}

void mg_sfx_preset_jump(mg_sfx_desc_t* out_desc) {
    if (!out_desc) return;
    memset(out_desc, 0, sizeof(mg_sfx_desc_t));
    out_desc->wave_type = MG_WAVE_SQUARE;
    out_desc->start_frequency = 150.0f;
    out_desc->end_frequency = 600.0f;
    out_desc->duration_sec = 0.18f;
    out_desc->attack_sec = 0.01f;
    out_desc->decay_sec = 0.05f;
    out_desc->sustain_level = 0.5f;
    out_desc->release_sec = 0.12f;
    out_desc->volume = 0.75f;
    out_desc->duty_cycle = 0.5f;
}

size_t mg_sfx_generate(
    const mg_sfx_desc_t* desc,
    float* out_buffer,
    size_t max_samples,
    uint32_t sample_rate
) {
    if (!desc || !out_buffer || max_samples == 0 || sample_rate == 0) return 0;

    size_t total_samples = (size_t)(desc->duration_sec * (float)sample_rate);
    if (total_samples > max_samples) total_samples = max_samples;
    if (total_samples == 0) return 0;

    float phase = 0.0f;
    float duty = (desc->duty_cycle >= 0.05f && desc->duty_cycle <= 0.95f) ? desc->duty_cycle : 0.5f;

    for (size_t i = 0; i < total_samples; ++i) {
        float t = (float)i / (float)sample_rate;
        float progress = (desc->duration_sec > 0.0f) ? (t / desc->duration_sec) : 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        /* Frequency Sweep */
        float freq = desc->start_frequency + progress * (desc->end_frequency - desc->start_frequency);
        if (freq < 10.0f) freq = 10.0f;

        float delta_phase = (2.0f * MG_PI * freq) / (float)sample_rate;
        phase = fmodf(phase + delta_phase, 2.0f * MG_PI);

        /* Evaluate Raw Waveform */
        float raw = 0.0f;
        switch (desc->wave_type) {
            case MG_WAVE_SINE:
                raw = sinf(phase);
                break;
            case MG_WAVE_SQUARE:
                raw = (phase < 2.0f * MG_PI * duty) ? 1.0f : -1.0f;
                break;
            case MG_WAVE_SAWTOOTH:
                raw = 1.0f - 2.0f * (phase / (2.0f * MG_PI));
                break;
            case MG_WAVE_NOISE:
                raw = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
                break;
        }

        /* Evaluate ADSR Envelope */
        float env = 0.0f;
        if (t < desc->attack_sec && desc->attack_sec > 0.0f) {
            env = t / desc->attack_sec;
        } else if (t < (desc->attack_sec + desc->decay_sec) && desc->decay_sec > 0.0f) {
            float dt = t - desc->attack_sec;
            env = 1.0f - (1.0f - desc->sustain_level) * (dt / desc->decay_sec);
        } else if (t < (desc->duration_sec - desc->release_sec)) {
            env = desc->sustain_level;
        } else if (desc->release_sec > 0.0f) {
            float rt = desc->duration_sec - t;
            env = desc->sustain_level * (rt / desc->release_sec);
            if (env < 0.0f) env = 0.0f;
        }

        out_buffer[i] = raw * env * desc->volume;
    }

    return total_samples;
}

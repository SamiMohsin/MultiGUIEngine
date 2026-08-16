/**
 * @file mg_spatial_fx.c
 * @brief MultiGUIEngine 3D Doppler Pitch Shifting & Acoustic Reverb Engine implementation.
 */
#include "mg/audio/spatial_fx.h"
#include <string.h>
#include <math.h>

float mg_doppler_calculate_pitch(
    const mg_doppler_entity_t* listener,
    const mg_doppler_entity_t* source,
    float doppler_factor
) {
    if (!listener || !source) return 1.0f;
    if (doppler_factor <= 0.0f) doppler_factor = 1.0f;

    float dx = listener->position.x - source->position.x;
    float dy = listener->position.y - source->position.y;
    float dz = listener->position.z - source->position.z;
    float dist = sqrtf(dx * dx + dy * dy + dz * dz);

    if (dist < 0.0001f) return 1.0f;

    float rx = dx / dist;
    float ry = dy / dist;
    float rz = dz / dist;

    /* Project velocities onto direction vector */
    float v_l = listener->velocity.x * rx + listener->velocity.y * ry + listener->velocity.z * rz;
    float v_s = source->velocity.x * rx + source->velocity.y * ry + source->velocity.z * rz;

    float c = MG_AUDIO_SPEED_OF_SOUND;

    /* Prevent divide by zero if source speed approaches sound speed */
    float denom = c - (doppler_factor * v_s);
    if (fabsf(denom) < 1.0f) {
        denom = (denom >= 0.0f) ? 1.0f : -1.0f;
    }

    float numer = c - (doppler_factor * v_l);
    float pitch = numer / denom;

    if (pitch < 0.1f) pitch = 0.1f;
    if (pitch > 10.0f) pitch = 10.0f;

    return pitch;
}

void mg_reverb_init(mg_reverb_state_t* reverb, const mg_reverb_config_t* config) {
    if (!reverb) return;
    memset(reverb, 0, sizeof(mg_reverb_state_t));
    if (config) {
        reverb->config = *config;
    } else {
        reverb->config.room_size = 0.5f;
        reverb->config.damping = 0.2f;
        reverb->config.wet_level = 0.3f;
        reverb->config.dry_level = 0.7f;
    }
}

void mg_reverb_process(
    mg_reverb_state_t* reverb,
    const float* in_samples_l,
    const float* in_samples_r,
    float* out_samples_l,
    float* out_samples_r,
    size_t sample_count
) {
    if (!reverb || !in_samples_l || !in_samples_r || !out_samples_l || !out_samples_r) return;

    size_t delay_len_l = (size_t)(1000.0f + reverb->config.room_size * 900.0f);
    size_t delay_len_r = (size_t)(1050.0f + reverb->config.room_size * 900.0f);
    float feedback = 0.4f + reverb->config.room_size * 0.4f;

    for (size_t i = 0; i < sample_count; ++i) {
        /* Left Channel */
        float delayed_l = reverb->delay_line_l[reverb->write_idx_l];
        reverb->delay_line_l[reverb->write_idx_l] = in_samples_l[i] + delayed_l * feedback * (1.0f - reverb->config.damping);
        out_samples_l[i] = in_samples_l[i] * reverb->config.dry_level + delayed_l * reverb->config.wet_level;

        reverb->write_idx_l = (reverb->write_idx_l + 1) % delay_len_l;

        /* Right Channel */
        float delayed_r = reverb->delay_line_r[reverb->write_idx_r];
        reverb->delay_line_r[reverb->write_idx_r] = in_samples_r[i] + delayed_r * feedback * (1.0f - reverb->config.damping);
        out_samples_r[i] = in_samples_r[i] * reverb->config.dry_level + delayed_r * reverb->config.wet_level;

        reverb->write_idx_r = (reverb->write_idx_r + 1) % delay_len_r;
    }
}

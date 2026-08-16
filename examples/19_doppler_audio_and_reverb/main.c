/**
 * @file main.c
 * @brief Example 19: 3D Doppler Frequency Pitch Shifting & Acoustic Reverb Engine.
 */
#include "mg/audio/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 19: 3D Doppler Pitch Shift & Acoustic Environmental Reverb ===\n");

    /* 1. Stationary Listener at origin */
    mg_doppler_entity_t listener = {
        .position = { 0.0f, 0.0f, 0.0f },
        .velocity = { 0.0f, 0.0f, 0.0f }
    };

    /* 2. Fast Projectile Approaching from +X at 100 m/s (~360 km/h) */
    mg_doppler_entity_t approaching_source = {
        .position = { 100.0f, 0.0f, 0.0f },
        .velocity = { -100.0f, 0.0f, 0.0f } /* Moving towards listener */
    };

    float pitch_approach = mg_doppler_calculate_pitch(&listener, &approaching_source, 1.0f);
    assert(pitch_approach > 1.0f);
    printf("PASS: Approaching Projectile Doppler Pitch: %.2fx (Frequency shifted up)\n", pitch_approach);

    /* 3. Fast Projectile Receding to -X at 100 m/s */
    mg_doppler_entity_t receding_source = {
        .position = { -100.0f, 0.0f, 0.0f },
        .velocity = { -100.0f, 0.0f, 0.0f } /* Moving away from listener */
    };

    float pitch_recede = mg_doppler_calculate_pitch(&listener, &receding_source, 1.0f);
    assert(pitch_recede < 1.0f);
    printf("PASS: Receding Projectile Doppler Pitch: %.2fx (Frequency shifted down)\n", pitch_recede);

    /* 4. Acoustic Reverb Simulation */
    mg_reverb_config_t config = {
        .room_size = 0.75f,
        .damping = 0.15f,
        .wet_level = 0.40f,
        .dry_level = 0.60f
    };

    mg_reverb_state_t reverb;
    mg_reverb_init(&reverb, &config);

    float in_l[128], in_r[128];
    float out_l[128], out_r[128];
    for (int i = 0; i < 128; ++i) {
        in_l[i] = (i == 0) ? 1.0f : 0.0f; /* Audio impulse */
        in_r[i] = (i == 0) ? 1.0f : 0.0f;
    }

    mg_reverb_process(&reverb, in_l, in_r, out_l, out_r, 128);

    assert(out_l[0] == 0.60f); /* Direct dry path = 1.0 * 0.60 */
    printf("PASS: Acoustic Reverb Processed (Dry impulse: %.2f)\n", out_l[0]);

    printf("PASS: Example 19: 3D Doppler Pitch Shift & Acoustic Reverb completed successfully\n");
    return 0;
}

/**
 * @file main.c
 * @brief Example 38: Dynamic Audio Occlusion & Acoustic Wall Low-Pass Attenuation.
 */
#include "mg/audio/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4410 /* 0.1s */

static float compute_rms(const float* buffer, size_t count) {
    if (!buffer || count == 0) return 0.0f;
    double sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        sum += (double)(buffer[i] * buffer[i]);
    }
    return (float)sqrt(sum / (double)count);
}

int main(void) {
    printf("=== Example 38: Dynamic Audio Occlusion & Acoustic Low-Pass Filter ===\n");

    mg_vec3_t emitter_pos = { 0.0f, 0.0f, 10.0f };
    mg_vec3_t listener_pos = { 0.0f, 0.0f, 0.0f };

    /* 1. Direct Line of Sight (Unobstructed Air) */
    float occ_air, cutoff_air, vol_air;
    mg_audio_calculate_occlusion(&emitter_pos, &listener_pos, 0.0f, MG_ACOUSTIC_MATERIAL_AIR,
                                &occ_air, &cutoff_air, &vol_air);
    printf("PASS: Direct Air -> Occlusion: %.2f, Cutoff: %.0f Hz, Vol: %.2f\n",
           occ_air, cutoff_air, vol_air);
    assert(occ_air == 0.0f && cutoff_air == 20000.0f && vol_air == 1.0f);

    /* 2. Heavy Concrete Wall (0.8m thickness) */
    float occ_wall, cutoff_wall, vol_wall;
    mg_audio_calculate_occlusion(&emitter_pos, &listener_pos, 0.8f, MG_ACOUSTIC_MATERIAL_CONCRETE,
                                &occ_wall, &cutoff_wall, &vol_wall);
    printf("PASS: Concrete Wall (0.8m) -> Occlusion: %.2f, Cutoff: %.0f Hz, Vol: %.2f\n",
           occ_wall, cutoff_wall, vol_wall);
    assert(occ_wall > 0.6f && cutoff_wall < 6000.0f && vol_wall < 0.5f);

    /* 3. Audio Buffer Low-Pass Filter Processing */
    float* dry_buffer = (float*)malloc(BUFFER_SIZE * sizeof(float));
    float* wet_buffer = (float*)malloc(BUFFER_SIZE * sizeof(float));
    assert(dry_buffer && wet_buffer);

    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        /* High-frequency test tone (5000 Hz) */
        dry_buffer[i] = sinf(2.0f * 3.14159265f * 5000.0f * ((float)i / SAMPLE_RATE));
        wet_buffer[i] = dry_buffer[i];
    }

    float dry_rms = compute_rms(dry_buffer, BUFFER_SIZE);

    /* Apply concrete wall low-pass attenuation */
    mg_audio_apply_occlusion_filter(wet_buffer, BUFFER_SIZE, cutoff_wall, vol_wall, SAMPLE_RATE);
    float wet_rms = compute_rms(wet_buffer, BUFFER_SIZE);

    printf("PASS: Processed 5kHz Tone -> Dry RMS: %.4f, Occluded/Muffled RMS: %.4f (Attenuation: %.1f%%)\n",
           dry_rms, wet_rms, (1.0f - (wet_rms / dry_rms)) * 100.0f);
    assert(wet_rms < dry_rms * 0.3f);

    /* Clean up */
    free(dry_buffer);
    free(wet_buffer);

    printf("PASS: Example 38: Dynamic Audio Occlusion completed successfully\n");
    return 0;
}

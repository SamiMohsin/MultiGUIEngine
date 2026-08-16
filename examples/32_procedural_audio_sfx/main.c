/**
 * @file main.c
 * @brief Example 32: Procedural Audio SFX Synthesis & ADSR Sound Generation.
 */
#include "mg/audio/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE (SAMPLE_RATE * 1) /* 1 second buffer */

static float compute_rms(const float* buffer, size_t count) {
    if (!buffer || count == 0) return 0.0f;
    double sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        sum += (double)(buffer[i] * buffer[i]);
    }
    return (float)sqrt(sum / (double)count);
}

int main(void) {
    printf("=== Example 32: Procedural Audio SFX Synthesis & ADSR Sound Generation ===\n");

    float* audio_buffer = (float*)malloc(BUFFER_SIZE * sizeof(float));
    assert(audio_buffer != NULL);

    /* 1. Synthesize Laser Blast Preset (Sawtooth frequency downward sweep 1200Hz -> 200Hz) */
    mg_sfx_desc_t laser_desc;
    mg_sfx_preset_laser(&laser_desc);
    size_t laser_samples = mg_sfx_generate(&laser_desc, audio_buffer, BUFFER_SIZE, SAMPLE_RATE);
    float laser_rms = compute_rms(audio_buffer, laser_samples);
    printf("PASS: Laser Blast Synthesized: %zu samples (%.2f s), RMS Energy = %.4f\n",
           laser_samples, (float)laser_samples / SAMPLE_RATE, laser_rms);
    assert(laser_samples > 0 && laser_rms > 0.05f);

    /* 2. Synthesize Mega Explosion Preset (Shaped White Noise with low-frequency rumble) */
    mg_sfx_desc_t expl_desc;
    mg_sfx_preset_explosion(&expl_desc);
    size_t expl_samples = mg_sfx_generate(&expl_desc, audio_buffer, BUFFER_SIZE, SAMPLE_RATE);
    float expl_rms = compute_rms(audio_buffer, expl_samples);
    printf("PASS: Explosion Synthesized: %zu samples (%.2f s), RMS Energy = %.4f\n",
           expl_samples, (float)expl_samples / SAMPLE_RATE, expl_rms);
    assert(expl_samples > 0 && expl_rms > 0.05f);

    /* 3. Synthesize Coin Pickup Preset (Dual Sine upward arpeggio 587Hz -> 880Hz) */
    mg_sfx_desc_t coin_desc;
    mg_sfx_preset_pickup_coin(&coin_desc);
    size_t coin_samples = mg_sfx_generate(&coin_desc, audio_buffer, BUFFER_SIZE, SAMPLE_RATE);
    float coin_rms = compute_rms(audio_buffer, coin_samples);
    printf("PASS: Coin Pickup Synthesized: %zu samples (%.2f s), RMS Energy = %.4f\n",
           coin_samples, (float)coin_samples / SAMPLE_RATE, coin_rms);
    assert(coin_samples > 0 && coin_rms > 0.05f);

    /* 4. Synthesize Jump Preset (Square wave upward sweep 150Hz -> 600Hz) */
    mg_sfx_desc_t jump_desc;
    mg_sfx_preset_jump(&jump_desc);
    size_t jump_samples = mg_sfx_generate(&jump_desc, audio_buffer, BUFFER_SIZE, SAMPLE_RATE);
    float jump_rms = compute_rms(audio_buffer, jump_samples);
    printf("PASS: Jump Sound Synthesized: %zu samples (%.2f s), RMS Energy = %.4f\n",
           jump_samples, (float)jump_samples / SAMPLE_RATE, jump_rms);
    assert(jump_samples > 0 && jump_rms > 0.05f);

    /* Cleanup */
    free(audio_buffer);

    printf("PASS: Example 32: Procedural Audio SFX Synthesis completed successfully\n");
    return 0;
}

/**
 * @file test_mg_audio.c
 * @brief Comprehensive headless unit tests for mg-audio.
 */
#include "mg/audio/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int test_audio_mixer(void) {
    printf("Testing audio mixer graph and rendering...\n");

    mg_audio_desc_t adesc = {
        .sample_rate = 44100,
        .channels = 2,
        .buffer_frames = 256
    };
    mg_audio_engine_t* engine = mg_audio_create(&adesc);
    if (!engine) return 1;

    /* Generate 1000 samples of a 440 Hz sine wave */
    float sine_samples[1000];
    for (int i = 0; i < 1000; ++i) {
        sine_samples[i] = sinf(2.0f * 3.14159265f * 440.0f * (float)i / 44100.0f);
    }

    mg_sound_desc_t sdesc = {
        .sample_rate = 44100,
        .channels = 1,
        .pcm_samples = sine_samples,
        .sample_count = 1000
    };
    mg_sound_handle_t sound = mg_audio_sound_create(engine, &sdesc);
    if (sound == MG_INVALID_AUDIO_HANDLE) return 1;

    mg_voice_handle_t voice = mg_audio_play(engine, sound, MG_AUDIO_BUS_SFX, 1.0f, 1.0f, 0.0f, true);
    if (voice == MG_INVALID_AUDIO_HANDLE) return 1;

    float pcm_out[512]; /* 256 frames * 2 channels */
    mg_audio_render_frames(engine, pcm_out, 256);

    /* Verify non-zero output rendered */
    float energy = 0.0f;
    for (int i = 0; i < 512; ++i) {
        energy += fabsf(pcm_out[i]);
    }

    if (energy < 1.0f) {
        fprintf(stderr, "FAIL: audio output silent (energy=%f)\n", energy);
        mg_audio_destroy(engine);
        return 1;
    }

    mg_audio_voice_stop(engine, voice);
    mg_audio_sound_destroy(engine, sound);
    mg_audio_destroy(engine);

    printf("PASS: audio mixer graph and rendering (energy=%f)\n", energy);
    return 0;
}

int main(void) {
    if (test_audio_mixer() != 0) return 1;

    printf("All mg-audio unit tests passed successfully!\n");
    return 0;
}

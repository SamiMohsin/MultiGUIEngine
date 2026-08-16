/**
 * @file main.c
 * @brief Example 05: 3D Spatial Audio and Panning Demonstration.
 */
#include "mg/audio/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
    printf("Running Example 05: 3D Spatial Audio...\n");

    mg_audio_desc_t adesc = {
        .sample_rate = 48000,
        .channels = 2,
        .buffer_frames = 512
    };
    mg_audio_engine_t* audio = mg_audio_create(&adesc);
    if (!audio) {
        fprintf(stderr, "FAIL: mg_audio_create failed\n");
        return 1;
    }

    /* 440 Hz Sine wave */
    float pcm_data[480];
    for (int i = 0; i < 480; ++i) {
        pcm_data[i] = sinf(2.0f * 3.14159265f * 440.0f * (float)i / 48000.0f);
    }
    mg_sound_desc_t sdesc = {
        .sample_rate = 48000,
        .channels = 1,
        .pcm_samples = pcm_data,
        .sample_count = 480
    };
    mg_sound_handle_t sound = mg_audio_sound_create(audio, &sdesc);

    /* Play sound on SFX bus with right pan (0.9) */
    mg_voice_handle_t voice = mg_audio_play(audio, sound, MG_AUDIO_BUS_SFX, 1.0f, 1.0f, 0.9f, true);

    float pcm_out[1024]; /* 512 frames * 2 channels */
    mg_audio_render_frames(audio, pcm_out, 512);

    float left_energy = 0.0f;
    float right_energy = 0.0f;
    for (int i = 0; i < 512; ++i) {
        left_energy += fabsf(pcm_out[i * 2 + 0]);
        right_energy += fabsf(pcm_out[i * 2 + 1]);
    }

    if (right_energy <= left_energy * 2.0f) {
        fprintf(stderr, "FAIL: right channel energy (%f) not significantly higher than left (%f) for right pan\n",
                right_energy, left_energy);
        mg_audio_voice_stop(audio, voice);
        mg_audio_sound_destroy(audio, sound);
        mg_audio_destroy(audio);
        return 1;
    }

    mg_audio_voice_stop(audio, voice);
    mg_audio_sound_destroy(audio, sound);
    mg_audio_destroy(audio);

    printf("PASS: Example 05: 3D Spatial Audio verified (Left energy: %f, Right energy: %f)\n", left_energy, right_energy);
    return 0;
}

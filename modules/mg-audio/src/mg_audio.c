/**
 * @file mg_audio.c
 * @brief MultiGUIEngine Audio Mixer and Playback Engine implementation.
 */
#include "mg/audio/audio.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MG_MAX_SOUNDS 256
#define MG_MAX_VOICES 64

typedef struct {
    float* pcm_samples;
    size_t sample_count;
    uint32_t sample_rate;
    uint32_t channels;
    bool is_active;
} mg_internal_sound_t;

typedef struct {
    mg_sound_handle_t sound;
    mg_audio_bus_t bus;
    float volume;
    float pitch;
    float pan; /* [-1.0 (left), 1.0 (right)] */
    bool loop;
    double cursor;
    bool is_playing;
} mg_internal_voice_t;

struct mg_audio_engine {
    mg_audio_desc_t desc;
    float bus_volumes[MG_AUDIO_BUS_COUNT];
    mg_internal_sound_t sounds[MG_MAX_SOUNDS];
    mg_internal_voice_t voices[MG_MAX_VOICES];
};

mg_audio_engine_t* mg_audio_create(const mg_audio_desc_t* desc) {
    mg_audio_engine_t* engine = (mg_audio_engine_t*)malloc(sizeof(mg_audio_engine_t));
    if (!engine) return NULL;

    memset(engine, 0, sizeof(mg_audio_engine_t));
    engine->desc = desc ? *desc : (mg_audio_desc_t){ 48000, 2, 512 };

    for (int i = 0; i < (int)MG_AUDIO_BUS_COUNT; ++i) {
        engine->bus_volumes[i] = 1.0f;
    }

    return engine;
}

void mg_audio_destroy(mg_audio_engine_t* engine) {
    if (!engine) return;

    for (size_t i = 0; i < MG_MAX_SOUNDS; ++i) {
        if (engine->sounds[i].is_active && engine->sounds[i].pcm_samples) {
            free(engine->sounds[i].pcm_samples);
        }
    }

    free(engine);
}

mg_sound_handle_t mg_audio_sound_create(mg_audio_engine_t* engine, const mg_sound_desc_t* desc) {
    if (!engine || !desc || !desc->pcm_samples || desc->sample_count == 0) {
        return MG_INVALID_AUDIO_HANDLE;
    }

    for (uint32_t i = 1; i < MG_MAX_SOUNDS; ++i) {
        if (!engine->sounds[i].is_active) {
            float* samples = (float*)malloc(desc->sample_count * sizeof(float));
            if (!samples) return MG_INVALID_AUDIO_HANDLE;

            memcpy(samples, desc->pcm_samples, desc->sample_count * sizeof(float));
            engine->sounds[i].pcm_samples = samples;
            engine->sounds[i].sample_count = desc->sample_count;
            engine->sounds[i].sample_rate = desc->sample_rate ? desc->sample_rate : 48000;
            engine->sounds[i].channels = desc->channels ? desc->channels : 1;
            engine->sounds[i].is_active = true;
            return i;
        }
    }
    return MG_INVALID_AUDIO_HANDLE;
}

void mg_audio_sound_destroy(mg_audio_engine_t* engine, mg_sound_handle_t sound) {
    if (engine && sound != MG_INVALID_AUDIO_HANDLE && sound < MG_MAX_SOUNDS) {
        if (engine->sounds[sound].is_active) {
            free(engine->sounds[sound].pcm_samples);
            engine->sounds[sound].pcm_samples = NULL;
            engine->sounds[sound].is_active = false;
        }
    }
}

mg_voice_handle_t mg_audio_play(
    mg_audio_engine_t* engine,
    mg_sound_handle_t sound,
    mg_audio_bus_t bus,
    float volume,
    float pitch,
    float pan,
    bool loop
) {
    if (!engine || sound == MG_INVALID_AUDIO_HANDLE || sound >= MG_MAX_SOUNDS) {
        return MG_INVALID_AUDIO_HANDLE;
    }
    if (!engine->sounds[sound].is_active) return MG_INVALID_AUDIO_HANDLE;

    for (uint32_t v = 1; v < MG_MAX_VOICES; ++v) {
        if (!engine->voices[v].is_playing) {
            engine->voices[v].sound = sound;
            engine->voices[v].bus = (bus < MG_AUDIO_BUS_COUNT) ? bus : MG_AUDIO_BUS_SFX;
            engine->voices[v].volume = fmaxf(volume, 0.0f);
            engine->voices[v].pitch = (pitch > 0.01f) ? pitch : 1.0f;
            engine->voices[v].pan = fmaxf(-1.0f, fminf(1.0f, pan));
            engine->voices[v].loop = loop;
            engine->voices[v].cursor = 0.0;
            engine->voices[v].is_playing = true;
            return v;
        }
    }
    return MG_INVALID_AUDIO_HANDLE;
}

void mg_audio_voice_stop(mg_audio_engine_t* engine, mg_voice_handle_t voice) {
    if (engine && voice != MG_INVALID_AUDIO_HANDLE && voice < MG_MAX_VOICES) {
        engine->voices[voice].is_playing = false;
    }
}

void mg_audio_set_bus_volume(mg_audio_engine_t* engine, mg_audio_bus_t bus, float volume) {
    if (engine && bus < MG_AUDIO_BUS_COUNT) {
        engine->bus_volumes[bus] = fmaxf(0.0f, volume);
    }
}

void mg_audio_render_frames(mg_audio_engine_t* engine, float* out_stereo_pcm, size_t frame_count) {
    if (!engine || !out_stereo_pcm || frame_count == 0) return;

    memset(out_stereo_pcm, 0, frame_count * 2 * sizeof(float));

    float master_vol = engine->bus_volumes[MG_AUDIO_BUS_MASTER];

    for (uint32_t v = 1; v < MG_MAX_VOICES; ++v) {
        mg_internal_voice_t* voice = &engine->voices[v];
        if (!voice->is_playing) continue;

        mg_internal_sound_t* sound = &engine->sounds[voice->sound];
        if (!sound->is_active || !sound->pcm_samples) {
            voice->is_playing = false;
            continue;
        }

        float bus_vol = engine->bus_volumes[voice->bus];
        float total_vol = master_vol * bus_vol * voice->volume;

        /* Pan calculation: pan in [-1, 1] */
        float left_gain = total_vol * fmaxf(0.0f, (1.0f - voice->pan) * 0.5f);
        float right_gain = total_vol * fmaxf(0.0f, (1.0f + voice->pan) * 0.5f);

        double step = (double)voice->pitch * ((double)sound->sample_rate / (double)engine->desc.sample_rate);

        for (size_t f = 0; f < frame_count; ++f) {
            size_t sample_idx = (size_t)voice->cursor;
            if (sample_idx >= sound->sample_count) {
                if (voice->loop) {
                    voice->cursor = fmod(voice->cursor, (double)sound->sample_count);
                    sample_idx = (size_t)voice->cursor;
                } else {
                    voice->is_playing = false;
                    break;
                }
            }

            float sample = sound->pcm_samples[sample_idx];
            out_stereo_pcm[f * 2 + 0] += sample * left_gain;
            out_stereo_pcm[f * 2 + 1] += sample * right_gain;

            voice->cursor += step;
        }
    }

    /* Soft limiter */
    for (size_t i = 0; i < frame_count * 2; ++i) {
        float x = out_stereo_pcm[i];
        out_stereo_pcm[i] = tanhf(x);
    }
}

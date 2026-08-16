/**
 * @file audio.h
 * @brief MultiGUIEngine Audio Mixer and Playback Engine.
 */
#ifndef MG_AUDIO_AUDIO_H
#define MG_AUDIO_AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "mg/audio/dsp.h"
#include "mg/audio/spatial_fx.h"
#include "mg/audio/music_sm.h"



#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t mg_sound_handle_t;
typedef uint32_t mg_voice_handle_t;

#define MG_INVALID_AUDIO_HANDLE 0

typedef enum mg_audio_bus {
    MG_AUDIO_BUS_MASTER = 0,
    MG_AUDIO_BUS_SFX,
    MG_AUDIO_BUS_MUSIC,
    MG_AUDIO_BUS_VOICE,
    MG_AUDIO_BUS_UI,
    MG_AUDIO_BUS_COUNT
} mg_audio_bus_t;

typedef struct mg_audio_desc {
    uint32_t sample_rate;    /**< e.g. 44100 or 48000 Hz. */
    uint32_t channels;       /**< 1 (mono) or 2 (stereo). */
    uint32_t buffer_frames;  /**< Frame buffer size (e.g. 512). */
} mg_audio_desc_t;

typedef struct mg_sound_desc {
    uint32_t sample_rate;
    uint32_t channels;
    const float* pcm_samples;
    size_t sample_count;
} mg_sound_desc_t;

typedef struct mg_audio_engine mg_audio_engine_t;

/**
 * @brief Initializes the audio engine.
 */
mg_audio_engine_t* mg_audio_create(const mg_audio_desc_t* desc);

/**
 * @brief Destroys the audio engine.
 */
void mg_audio_destroy(mg_audio_engine_t* engine);

/**
 * @brief Loads a raw PCM audio clip.
 */
mg_sound_handle_t mg_audio_sound_create(mg_audio_engine_t* engine, const mg_sound_desc_t* desc);

/**
 * @brief Destroys an audio clip.
 */
void mg_audio_sound_destroy(mg_audio_engine_t* engine, mg_sound_handle_t sound);

/**
 * @brief Plays a sound clip on an audio bus.
 * @return Voice handle representing the active playing instance.
 */
mg_voice_handle_t mg_audio_play(
    mg_audio_engine_t* engine,
    mg_sound_handle_t sound,
    mg_audio_bus_t bus,
    float volume,
    float pitch,
    float pan,
    bool loop
);

/**
 * @brief Stops an active voice.
 */
void mg_audio_voice_stop(mg_audio_engine_t* engine, mg_voice_handle_t voice);

/**
 * @brief Sets master or bus volume [0.0, 1.0].
 */
void mg_audio_set_bus_volume(mg_audio_engine_t* engine, mg_audio_bus_t bus, float volume);

/**
 * @brief Renders the next audio chunk into a stereo interleaved float buffer.
 * @param engine Pointer to engine.
 * @param out_stereo_pcm Interleaved stereo output buffer.
 * @param frame_count Number of audio frames to render.
 */
void mg_audio_render_frames(mg_audio_engine_t* engine, float* out_stereo_pcm, size_t frame_count);

#ifdef __cplusplus
}
#endif

#endif /* MG_AUDIO_AUDIO_H */

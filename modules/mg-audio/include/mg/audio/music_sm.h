/**
 * @file music_sm.h
 * @brief MultiGUIEngine Interactive Dynamic Music State Machine & Equal-Power Cross-Fading.
 */
#ifndef MG_AUDIO_MUSIC_SM_H
#define MG_AUDIO_MUSIC_SM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_MUSIC_MAX_TRACKS 16

typedef struct mg_music_track {
    uint32_t track_id;
    char name[64];
    float base_volume;
    float current_volume;
} mg_music_track_t;

typedef struct mg_music_state_machine {
    mg_music_track_t tracks[MG_MUSIC_MAX_TRACKS];
    uint32_t track_count;
    int32_t current_track_idx;
    int32_t target_track_idx;
    float fade_timer;
    float fade_duration;
    bool is_crossfading;
} mg_music_state_machine_t;

/**
 * @brief Initializes a dynamic music state machine.
 */
void mg_music_sm_init(mg_music_state_machine_t* sm);

/**
 * @brief Registers a track stem into the state machine.
 */
bool mg_music_sm_add_track(
    mg_music_state_machine_t* sm,
    uint32_t track_id,
    const char* name,
    float base_volume
);

/**
 * @brief Begins an equal-power cross-fade transition to a target track.
 */
void mg_music_sm_transition(
    mg_music_state_machine_t* sm,
    uint32_t target_track_id,
    float fade_duration
);

/**
 * @brief Updates the music cross-fade timer and evaluates equal-power gain curves.
 */
void mg_music_sm_update(mg_music_state_machine_t* sm, float dt);

/**
 * @brief Queries current active playback volume for a specific track.
 */
float mg_music_sm_get_track_volume(const mg_music_state_machine_t* sm, uint32_t track_id);

#ifdef __cplusplus
}
#endif

#endif /* MG_AUDIO_MUSIC_SM_H */

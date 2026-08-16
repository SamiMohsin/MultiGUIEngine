/**
 * @file mg_music_sm.c
 * @brief MultiGUIEngine Interactive Dynamic Music State Machine & Equal-Power Cross-Fading implementation.
 */
#include "mg/audio/music_sm.h"
#include <string.h>
#include <math.h>

#define MG_PI_2 1.57079632679f

void mg_music_sm_init(mg_music_state_machine_t* sm) {
    if (!sm) return;
    memset(sm, 0, sizeof(mg_music_state_machine_t));
    sm->current_track_idx = -1;
    sm->target_track_idx = -1;
}

bool mg_music_sm_add_track(
    mg_music_state_machine_t* sm,
    uint32_t track_id,
    const char* name,
    float base_volume
) {
    if (!sm || !name || sm->track_count >= MG_MUSIC_MAX_TRACKS) return false;

    mg_music_track_t* track = &sm->tracks[sm->track_count++];
    track->track_id = track_id;
    strncpy(track->name, name, sizeof(track->name) - 1);
    track->base_volume = base_volume;
    track->current_volume = 0.0f;

    /* If first track, set as active immediately */
    if (sm->current_track_idx == -1) {
        sm->current_track_idx = 0;
        track->current_volume = base_volume;
    }

    return true;
}

void mg_music_sm_transition(
    mg_music_state_machine_t* sm,
    uint32_t target_track_id,
    float fade_duration
) {
    if (!sm || fade_duration <= 0.0f) return;

    int32_t target_idx = -1;
    for (uint32_t i = 0; i < sm->track_count; ++i) {
        if (sm->tracks[i].track_id == target_track_id) {
            target_idx = (int32_t)i;
            break;
        }
    }

    if (target_idx == -1 || target_idx == sm->current_track_idx) return;

    sm->target_track_idx = target_idx;
    sm->fade_duration = fade_duration;
    sm->fade_timer = 0.0f;
    sm->is_crossfading = true;
}

void mg_music_sm_update(mg_music_state_machine_t* sm, float dt) {
    if (!sm) return;

    if (!sm->is_crossfading) {
        for (uint32_t i = 0; i < sm->track_count; ++i) {
            if ((int32_t)i == sm->current_track_idx) {
                sm->tracks[i].current_volume = sm->tracks[i].base_volume;
            } else {
                sm->tracks[i].current_volume = 0.0f;
            }
        }
        return;
    }

    sm->fade_timer += dt;
    float p = sm->fade_timer / sm->fade_duration;
    if (p >= 1.0f) {
        sm->is_crossfading = false;
        sm->current_track_idx = sm->target_track_idx;
        sm->target_track_idx = -1;

        for (uint32_t i = 0; i < sm->track_count; ++i) {
            if ((int32_t)i == sm->current_track_idx) {
                sm->tracks[i].current_volume = sm->tracks[i].base_volume;
            } else {
                sm->tracks[i].current_volume = 0.0f;
            }
        }
        return;
    }

    /* Equal-power cross-fade curves: cos(p * pi/2) for out, sin(p * pi/2) for in */
    float fade_out_gain = cosf(p * MG_PI_2);
    float fade_in_gain  = sinf(p * MG_PI_2);

    for (uint32_t i = 0; i < sm->track_count; ++i) {
        if ((int32_t)i == sm->current_track_idx) {
            sm->tracks[i].current_volume = sm->tracks[i].base_volume * fade_out_gain;
        } else if ((int32_t)i == sm->target_track_idx) {
            sm->tracks[i].current_volume = sm->tracks[i].base_volume * fade_in_gain;
        } else {
            sm->tracks[i].current_volume = 0.0f;
        }
    }

}

float mg_music_sm_get_track_volume(const mg_music_state_machine_t* sm, uint32_t track_id) {
    if (!sm) return 0.0f;

    for (uint32_t i = 0; i < sm->track_count; ++i) {
        if (sm->tracks[i].track_id == track_id) {
            return sm->tracks[i].current_volume;
        }
    }
    return 0.0f;
}

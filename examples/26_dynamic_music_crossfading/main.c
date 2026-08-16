/**
 * @file main.c
 * @brief Example 26: Interactive Dynamic Music State Machine & Equal-Power Cross-Fading.
 */
#include "mg/audio/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define TRACK_EXPLORATION 101
#define TRACK_COMBAT      102
#define TRACK_VICTORY     103

int main(void) {
    printf("=== Example 26: Interactive Dynamic Music State Machine & Equal-Power Cross-Fading ===\n");

    /* 1. Initialize State Machine */
    mg_music_state_machine_t sm;
    mg_music_sm_init(&sm);

    /* 2. Register Soundtrack Stems */
    mg_music_sm_add_track(&sm, TRACK_EXPLORATION, "Track_Exploration_Ambient", 1.0f);
    mg_music_sm_add_track(&sm, TRACK_COMBAT,      "Track_Combat_High_Intensity", 1.0f);
    mg_music_sm_add_track(&sm, TRACK_VICTORY,     "Track_Boss_Victory_Fanfare", 0.9f);
    printf("PASS: Registered 3 Soundtrack Stems. Initial Active: TRACK_EXPLORATION (vol = %.2f)\n",
           mg_music_sm_get_track_volume(&sm, TRACK_EXPLORATION));
    assert(mg_music_sm_get_track_volume(&sm, TRACK_EXPLORATION) == 1.0f);
    assert(mg_music_sm_get_track_volume(&sm, TRACK_COMBAT) == 0.0f);

    /* 3. Trigger 2.0s Transition into Combat Track */
    printf("--> Triggering Equal-Power Transition to TRACK_COMBAT (fade_duration = 2.0s)...\n");
    mg_music_sm_transition(&sm, TRACK_COMBAT, 2.0f);

    /* 4. Advance time to 50% midpoint (1.0s) */
    mg_music_sm_update(&sm, 1.0f);
    float exp_vol = mg_music_sm_get_track_volume(&sm, TRACK_EXPLORATION);
    float com_vol = mg_music_sm_get_track_volume(&sm, TRACK_COMBAT);
    printf("PASS: Midpoint (50%%) Cross-Fade Volumes -> Exploration: %.3f, Combat: %.3f\n", exp_vol, com_vol);
    /* At 50%, cos(pi/4) = sin(pi/4) = 0.7071 (Equal Power sum: 0.7071^2 + 0.7071^2 = 1.0) */
    float power_sum = exp_vol * exp_vol + com_vol * com_vol;
    printf("PASS: Equal-Power Acoustic Energy Sum: %.4f (100%% Constant Power)\n", power_sum);
    assert(fabsf(power_sum - 1.0f) < 0.01f);

    /* 5. Complete Transition (+1.0s -> 2.0s total) */
    mg_music_sm_update(&sm, 1.0f);
    assert(mg_music_sm_get_track_volume(&sm, TRACK_EXPLORATION) == 0.0f);
    assert(mg_music_sm_get_track_volume(&sm, TRACK_COMBAT) == 1.0f);
    printf("PASS: Transition Completed. TRACK_COMBAT is now 100%% active.\n");

    /* 6. Transition to Victory Fanfare (1.0s fast cross-fade) */
    mg_music_sm_transition(&sm, TRACK_VICTORY, 1.0f);
    mg_music_sm_update(&sm, 1.0f);
    assert(mg_music_sm_get_track_volume(&sm, TRACK_COMBAT) == 0.0f);
    assert(fabsf(mg_music_sm_get_track_volume(&sm, TRACK_VICTORY) - 0.9f) < 0.01f);
    printf("PASS: Victory Fanfare Transition Completed (vol = %.2f)\n",
           mg_music_sm_get_track_volume(&sm, TRACK_VICTORY));

    printf("PASS: Example 26: Dynamic Music Cross-Fading completed successfully\n");
    return 0;
}

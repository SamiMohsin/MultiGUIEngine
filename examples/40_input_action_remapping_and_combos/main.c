/**
 * @file main.c
 * @brief Example 40: Input Action Remapping, Chords & Input Buffering.
 */
#include "mg/input/input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>


#define KEY_SPACE  32
#define KEY_A      65
#define KEY_D      68
#define KEY_S      83
#define KEY_W      87
#define KEY_F      70
#define KEY_CTRL   17
#define KEY_GAMEPAD_A 300

int main(void) {
    printf("=== Example 40: Input Action Remapping, Chords & 2D Vector Axes ===\n");

    /* 1. Initialize Action Map */
    mg_input_action_map_t map;
    mg_input_action_map_init(&map);

    /* 2. Bind Digital Actions & Chords */
    mg_input_action_bind_button(&map, "Jump", KEY_SPACE);
    mg_input_action_bind_button(&map, "Fire", KEY_F);
    mg_input_action_bind_chord(&map, "SuperLaser", KEY_CTRL, KEY_F);
    mg_input_action_bind_axis_2d(&map, "Move", KEY_W, KEY_S, KEY_A, KEY_D);

    printf("PASS: Registered 4 Gameplay Actions (Jump, Fire, SuperLaser Chord, Move 2D Axis)\n");

    /* 3. Test Normal Button Trigger */
    mg_input_action_map_feed_key(&map, KEY_SPACE, true);
    assert(mg_input_action_is_triggered(&map, "Jump") == true);
    assert(mg_input_action_is_triggered(&map, "Fire") == false);

    mg_input_action_map_feed_key(&map, KEY_SPACE, false);
    assert(mg_input_action_is_triggered(&map, "Jump") == false);
    printf("PASS: Digital Button 'Jump' triggered & released accurately\n");

    /* 4. Test Chord Combo: Pressing F alone vs Ctrl + F */
    mg_input_action_map_feed_key(&map, KEY_F, true);
    assert(mg_input_action_is_triggered(&map, "Fire") == true);
    assert(mg_input_action_is_triggered(&map, "SuperLaser") == false); /* Ctrl not pressed yet */

    mg_input_action_map_feed_key(&map, KEY_CTRL, true);
    assert(mg_input_action_is_triggered(&map, "SuperLaser") == true);  /* Both Ctrl + F pressed */
    printf("PASS: Chord Combo 'SuperLaser' (Ctrl+F) evaluated accurately\n");

    mg_input_action_map_feed_key(&map, KEY_F, false);
    mg_input_action_map_feed_key(&map, KEY_CTRL, false);

    /* 5. Test 2D Composite Vector Axis Normalization (Diagonal W + D) */
    mg_input_action_map_feed_key(&map, KEY_W, true); /* Up */
    mg_input_action_map_feed_key(&map, KEY_D, true); /* Right */

    float ax = 0.0f, ay = 0.0f;
    mg_input_action_get_axis_2d(&map, "Move", &ax, &ay);
    float length = sqrtf(ax * ax + ay * ay);

    printf("PASS: Diagonal Move (W+D) -> Vector: (%.4f, %.4f), Normalized Magnitude = %.4f\n",
           ax, ay, length);
    assert(fabsf(length - 1.0f) < 0.001f);
    assert(fabsf(ax - 0.7071f) < 0.001f);
    assert(fabsf(ay - 0.7071f) < 0.001f);

    /* 6. Dynamic Remapping: Remap 'Jump' from Space to Gamepad A */
    for (uint32_t i = 0; i < map.action_count; ++i) {
        if (strcmp(map.actions[i].name, "Jump") == 0) {
            map.actions[i].primary_key = KEY_GAMEPAD_A;
            break;
        }
    }

    mg_input_action_map_feed_key(&map, KEY_SPACE, true);
    assert(mg_input_action_is_triggered(&map, "Jump") == false); /* Space no longer triggers Jump */

    mg_input_action_map_feed_key(&map, KEY_GAMEPAD_A, true);
    assert(mg_input_action_is_triggered(&map, "Jump") == true);  /* Gamepad A triggers Jump */
    printf("PASS: Live Action Remapping verified (Jump remapped to Gamepad Button A)\n");

    printf("PASS: Example 40: Input Action Remapping completed successfully\n");
    return 0;
}

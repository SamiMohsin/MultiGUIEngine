/**
 * @file main.c
 * @brief Example 16: Gamepad Deadzones, Virtual Touch Joystick & Gesture Recognition.
 */
#include "mg/input/input.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    printf("=== Example 16: Gamepad, Virtual Touch Joystick & Gesture Recognition ===\n");

    /* 1. Test Gamepad Radial Deadzone Filtering */
    mg_vec2_t stick_drift = { 0.05f, 0.08f }; /* Inside deadzone threshold */
    mg_vec2_t filtered_drift = mg_gamepad_filter_deadzone(&stick_drift, 0.15f);
    assert(filtered_drift.x == 0.0f && filtered_drift.y == 0.0f);

    mg_vec2_t stick_tilt = { 0.6f, 0.8f }; /* Active input magnitude = 1.0 */
    mg_vec2_t filtered_tilt = mg_gamepad_filter_deadzone(&stick_tilt, 0.15f);
    assert(filtered_tilt.x > 0.0f && filtered_tilt.y > 0.0f);
    printf("PASS: Gamepad Radial Deadzone Filtering verified (Drift filtered, Input mapped: %.2f, %.2f)\n",
           filtered_tilt.x, filtered_tilt.y);

    /* 2. Test Virtual Touch Joystick */
    mg_virtual_joystick_t joy;
    mg_virtual_joystick_init(&joy, 150.0f, 600.0f, 50.0f);

    /* Touch pushed to the right (+60px > 50px radius clamp) */
    mg_vec2_t joy_axis = mg_virtual_joystick_update(&joy, 210.0f, 600.0f, true);
    assert(joy.is_active == true);
    assert(joy_axis.x > 0.99f && joy_axis.x <= 1.0f);
    assert(joy.thumb_pos.x == 200.0f); /* Clamped to center.x + radius */
    printf("PASS: Virtual Touch Joystick verified (Clamped vector: %.2f, %.2f)\n", joy_axis.x, joy_axis.y);

    /* 3. Test Touch Gesture Recognizer (Swipe & Tap) */
    mg_vec2_t p_start = { 100.0f, 200.0f };
    mg_vec2_t p_tap_end = { 105.0f, 203.0f };
    mg_gesture_event_t tap_ev = mg_gesture_recognize_swipe(&p_start, &p_tap_end, 20.0f);
    assert(tap_ev.type == MG_GESTURE_TAP);
    printf("PASS: Touch Tap Gesture recognized\n");

    mg_vec2_t p_swipe_right = { 300.0f, 210.0f };
    mg_gesture_event_t swipe_ev = mg_gesture_recognize_swipe(&p_start, &p_swipe_right, 20.0f);
    assert(swipe_ev.type == MG_GESTURE_SWIPE_RIGHT);
    printf("PASS: Touch Swipe Right Gesture recognized\n");

    mg_vec2_t p_swipe_up = { 102.0f, 50.0f };
    mg_gesture_event_t swipe_up_ev = mg_gesture_recognize_swipe(&p_start, &p_swipe_up, 20.0f);
    assert(swipe_up_ev.type == MG_GESTURE_SWIPE_UP);
    printf("PASS: Touch Swipe Up Gesture recognized\n");

    printf("PASS: Example 16: Gamepad, Virtual Joystick & Gestures completed successfully\n");
    return 0;
}

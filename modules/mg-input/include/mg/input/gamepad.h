/**
 * @file gamepad.h
 * @brief MultiGUIEngine Gamepad, Virtual Touch Joystick, and Touch Gesture Recognition.
 */
#ifndef MG_INPUT_GAMEPAD_H
#define MG_INPUT_GAMEPAD_H

#include "mg/math/vec2.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_gamepad_button {
    MG_GAMEPAD_BTN_A = 0,
    MG_GAMEPAD_BTN_B,
    MG_GAMEPAD_BTN_X,
    MG_GAMEPAD_BTN_Y,
    MG_GAMEPAD_BTN_LB,
    MG_GAMEPAD_BTN_RB,
    MG_GAMEPAD_BTN_BACK,
    MG_GAMEPAD_BTN_START,
    MG_GAMEPAD_BTN_GUIDE,
    MG_GAMEPAD_BTN_LS,
    MG_GAMEPAD_BTN_RS,
    MG_GAMEPAD_BTN_DPAD_UP,
    MG_GAMEPAD_BTN_DPAD_DOWN,
    MG_GAMEPAD_BTN_DPAD_LEFT,
    MG_GAMEPAD_BTN_DPAD_RIGHT,
    MG_GAMEPAD_BTN_COUNT
} mg_gamepad_button_t;

typedef struct mg_gamepad_state {
    mg_vec2_t left_stick;   /* [-1.0, 1.0] */
    mg_vec2_t right_stick;  /* [-1.0, 1.0] */
    float left_trigger;     /* [0.0, 1.0] */
    float right_trigger;    /* [0.0, 1.0] */
    bool buttons[MG_GAMEPAD_BTN_COUNT];
} mg_gamepad_state_t;

typedef struct mg_virtual_joystick {
    mg_vec2_t center;
    float radius;
    mg_vec2_t thumb_pos;
    bool is_active;
} mg_virtual_joystick_t;

typedef enum mg_gesture_type {
    MG_GESTURE_NONE = 0,
    MG_GESTURE_TAP,
    MG_GESTURE_SWIPE_LEFT,
    MG_GESTURE_SWIPE_RIGHT,
    MG_GESTURE_SWIPE_UP,
    MG_GESTURE_SWIPE_DOWN,
    MG_GESTURE_PINCH
} mg_gesture_type_t;

typedef struct mg_gesture_event {
    mg_gesture_type_t type;
    mg_vec2_t position;
    float scale_factor; /* For pinch gestures */
} mg_gesture_event_t;

/**
 * @brief Applies radial deadzone filtering to an analog stick vector.
 */
mg_vec2_t mg_gamepad_filter_deadzone(const mg_vec2_t* raw_stick, float deadzone);

/**
 * @brief Initializes a virtual touch joystick.
 */
void mg_virtual_joystick_init(mg_virtual_joystick_t* joy, float center_x, float center_y, float radius);

/**
 * @brief Updates touch position for a virtual joystick and returns normalized direction [-1.0, 1.0].
 */
mg_vec2_t mg_virtual_joystick_update(mg_virtual_joystick_t* joy, float touch_x, float touch_y, bool is_touching);

/**
 * @brief Evaluates touch points to recognize gestures (swipe, pinch).
 */
mg_gesture_event_t mg_gesture_recognize_swipe(const mg_vec2_t* start_pos, const mg_vec2_t* end_pos, float min_dist);

#ifdef __cplusplus
}
#endif

#endif /* MG_INPUT_GAMEPAD_H */

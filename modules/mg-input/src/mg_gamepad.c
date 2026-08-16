/**
 * @file mg_gamepad.c
 * @brief MultiGUIEngine Gamepad, Virtual Touch Joystick & Gesture Recognition implementation.
 */
#include "mg/input/gamepad.h"
#include <math.h>

mg_vec2_t mg_gamepad_filter_deadzone(const mg_vec2_t* raw_stick, float deadzone) {
    if (!raw_stick) return (mg_vec2_t){ 0.0f, 0.0f };
    if (deadzone <= 0.0f) return *raw_stick;

    float mag = sqrtf(raw_stick->x * raw_stick->x + raw_stick->y * raw_stick->y);
    if (mag < deadzone) {
        return (mg_vec2_t){ 0.0f, 0.0f };
    }

    float norm_mag = (mag - deadzone) / (1.0f - deadzone);
    if (norm_mag > 1.0f) norm_mag = 1.0f;

    return (mg_vec2_t){
        (raw_stick->x / mag) * norm_mag,
        (raw_stick->y / mag) * norm_mag
    };
}

void mg_virtual_joystick_init(mg_virtual_joystick_t* joy, float center_x, float center_y, float radius) {
    if (!joy) return;
    joy->center = (mg_vec2_t){ center_x, center_y };
    joy->radius = (radius > 0.0f) ? radius : 50.0f;
    joy->thumb_pos = joy->center;
    joy->is_active = false;
}

mg_vec2_t mg_virtual_joystick_update(mg_virtual_joystick_t* joy, float touch_x, float touch_y, bool is_touching) {
    if (!joy) return (mg_vec2_t){ 0.0f, 0.0f };

    if (!is_touching) {
        joy->is_active = false;
        joy->thumb_pos = joy->center;
        return (mg_vec2_t){ 0.0f, 0.0f };
    }

    joy->is_active = true;
    float dx = touch_x - joy->center.x;
    float dy = touch_y - joy->center.y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist > joy->radius) {
        joy->thumb_pos.x = joy->center.x + (dx / dist) * joy->radius;
        joy->thumb_pos.y = joy->center.y + (dy / dist) * joy->radius;
        return (mg_vec2_t){ dx / dist, dy / dist };
    }

    joy->thumb_pos = (mg_vec2_t){ touch_x, touch_y };
    return (mg_vec2_t){ dx / joy->radius, dy / joy->radius };
}

mg_gesture_event_t mg_gesture_recognize_swipe(const mg_vec2_t* start_pos, const mg_vec2_t* end_pos, float min_dist) {
    mg_gesture_event_t ev;
    ev.type = MG_GESTURE_NONE;
    ev.position = (mg_vec2_t){ 0.0f, 0.0f };
    ev.scale_factor = 1.0f;

    if (!start_pos || !end_pos) return ev;
    if (min_dist <= 0.0f) min_dist = 20.0f;

    float dx = end_pos->x - start_pos->x;
    float dy = end_pos->y - start_pos->y;
    float dist = sqrtf(dx * dx + dy * dy);

    ev.position = *end_pos;

    if (dist < min_dist) {
        ev.type = MG_GESTURE_TAP;
        return ev;
    }

    if (fabsf(dx) > fabsf(dy)) {
        ev.type = (dx > 0.0f) ? MG_GESTURE_SWIPE_RIGHT : MG_GESTURE_SWIPE_LEFT;
    } else {
        ev.type = (dy > 0.0f) ? MG_GESTURE_SWIPE_DOWN : MG_GESTURE_SWIPE_UP;
    }

    return ev;
}

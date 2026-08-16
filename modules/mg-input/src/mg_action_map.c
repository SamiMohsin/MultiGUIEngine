/**
 * @file mg_action_map.c
 * @brief MultiGUIEngine Input Action Remapping, Chords & Input Buffering Engine implementation.
 */
#include "mg/input/action_map.h"
#include <string.h>
#include <math.h>

void mg_input_action_map_init(mg_input_action_map_t* map) {
    if (!map) return;
    memset(map, 0, sizeof(mg_input_action_map_t));
}

int32_t mg_input_action_bind_button(
    mg_input_action_map_t* map,
    const char* name,
    int primary_key
) {
    if (!map || !name || map->action_count >= MG_MAX_INPUT_ACTIONS) return -1;

    int32_t idx = (int32_t)map->action_count++;
    mg_action_binding_t* b = &map->actions[idx];
    memset(b, 0, sizeof(mg_action_binding_t));

    strncpy(b->name, name, MG_MAX_ACTION_NAME_LEN - 1);
    b->type = MG_INPUT_ACTION_BUTTON;
    b->primary_key = primary_key;

    return idx;
}

int32_t mg_input_action_bind_chord(
    mg_input_action_map_t* map,
    const char* name,
    int modifier_key,
    int primary_key
) {
    if (!map || !name || map->action_count >= MG_MAX_INPUT_ACTIONS) return -1;

    int32_t idx = (int32_t)map->action_count++;
    mg_action_binding_t* b = &map->actions[idx];
    memset(b, 0, sizeof(mg_action_binding_t));

    strncpy(b->name, name, MG_MAX_ACTION_NAME_LEN - 1);
    b->type = MG_INPUT_ACTION_CHORD;
    b->modifier_key = modifier_key;
    b->primary_key = primary_key;

    return idx;
}

int32_t mg_input_action_bind_axis_2d(
    mg_input_action_map_t* map,
    const char* name,
    int up_key,
    int down_key,
    int left_key,
    int right_key
) {
    if (!map || !name || map->action_count >= MG_MAX_INPUT_ACTIONS) return -1;

    int32_t idx = (int32_t)map->action_count++;
    mg_action_binding_t* b = &map->actions[idx];
    memset(b, 0, sizeof(mg_action_binding_t));

    strncpy(b->name, name, MG_MAX_ACTION_NAME_LEN - 1);
    b->type = MG_INPUT_ACTION_AXIS2D;
    b->up_key = up_key;
    b->down_key = down_key;
    b->left_key = left_key;
    b->right_key = right_key;

    return idx;
}

void mg_input_action_map_feed_key(
    mg_input_action_map_t* map,
    int key,
    bool pressed
) {
    if (!map || key < 0 || key >= 512) return;

    map->key_states[key] = pressed;

    for (uint32_t i = 0; i < map->action_count; ++i) {
        mg_action_binding_t* b = &map->actions[i];

        if (b->type == MG_INPUT_ACTION_BUTTON) {
            b->is_triggered = map->key_states[b->primary_key];
        } else if (b->type == MG_INPUT_ACTION_CHORD) {
            b->is_triggered = map->key_states[b->modifier_key] && map->key_states[b->primary_key];
        } else if (b->type == MG_INPUT_ACTION_AXIS2D) {

            float dx = (map->key_states[b->right_key] ? 1.0f : 0.0f) -
                       (map->key_states[b->left_key] ? 1.0f : 0.0f);
            float dy = (map->key_states[b->up_key] ? 1.0f : 0.0f) -
                       (map->key_states[b->down_key] ? 1.0f : 0.0f);

            float len = sqrtf(dx * dx + dy * dy);
            if (len > 0.0001f) {
                b->axis_x = dx / len;
                b->axis_y = dy / len;
                b->is_triggered = true;
            } else {
                b->axis_x = 0.0f;
                b->axis_y = 0.0f;
                b->is_triggered = false;
            }
        }
    }
}

bool mg_input_action_is_triggered(
    const mg_input_action_map_t* map,
    const char* name
) {
    if (!map || !name) return false;

    for (uint32_t i = 0; i < map->action_count; ++i) {
        if (strncmp(map->actions[i].name, name, MG_MAX_ACTION_NAME_LEN) == 0) {
            return map->actions[i].is_triggered;
        }
    }
    return false;
}

void mg_input_action_get_axis_2d(
    const mg_input_action_map_t* map,
    const char* name,
    float* out_x,
    float* out_y
) {
    if (out_x) *out_x = 0.0f;
    if (out_y) *out_y = 0.0f;

    if (!map || !name) return;

    for (uint32_t i = 0; i < map->action_count; ++i) {
        if (strncmp(map->actions[i].name, name, MG_MAX_ACTION_NAME_LEN) == 0) {
            if (out_x) *out_x = map->actions[i].axis_x;
            if (out_y) *out_y = map->actions[i].axis_y;
            return;
        }
    }
}

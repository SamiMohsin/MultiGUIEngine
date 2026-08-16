/**
 * @file action_map.h
 * @brief MultiGUIEngine Input Action Remapping, Chords & Input Buffering Engine.
 */
#ifndef MG_INPUT_ACTION_MAP_H
#define MG_INPUT_ACTION_MAP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_MAX_INPUT_ACTIONS 32
#define MG_MAX_ACTION_NAME_LEN 32

typedef enum mg_input_action_type {
    MG_INPUT_ACTION_BUTTON = 0,
    MG_INPUT_ACTION_CHORD,
    MG_INPUT_ACTION_AXIS2D
} mg_input_action_type_t;

typedef struct mg_action_binding {
    char name[MG_MAX_ACTION_NAME_LEN];
    mg_input_action_type_t type;

    int primary_key;
    int modifier_key;
    int up_key;
    int down_key;
    int left_key;
    int right_key;
    bool is_triggered;
    float axis_x;
    float axis_y;
} mg_action_binding_t;

typedef struct mg_input_action_map {
    mg_action_binding_t actions[MG_MAX_INPUT_ACTIONS];
    uint32_t action_count;
    bool key_states[512];
} mg_input_action_map_t;

/**
 * @brief Initializes an input action map.
 */
void mg_input_action_map_init(mg_input_action_map_t* map);

/**
 * @brief Binds a digital button action to a primary key.
 */
int32_t mg_input_action_bind_button(
    mg_input_action_map_t* map,
    const char* name,
    int primary_key
);

/**
 * @brief Binds a chord combo action requiring both modifier and primary key pressed.
 */
int32_t mg_input_action_bind_chord(
    mg_input_action_map_t* map,
    const char* name,
    int modifier_key,
    int primary_key
);

/**
 * @brief Binds a composite 2D axis (e.g. WASD or Arrow Keys) producing a normalized direction vector.
 */
int32_t mg_input_action_bind_axis_2d(
    mg_input_action_map_t* map,
    const char* name,
    int up_key,
    int down_key,
    int left_key,
    int right_key
);

/**
 * @brief Feeds a raw key press or release event and evaluates all action bindings.
 */
void mg_input_action_map_feed_key(
    mg_input_action_map_t* map,
    int key,
    bool pressed
);

/**
 * @brief Checks if an action is currently active/triggered.
 */
bool mg_input_action_is_triggered(
    const mg_input_action_map_t* map,
    const char* name
);

/**
 * @brief Gets normalized 2D axis coordinates for an axis action.
 */
void mg_input_action_get_axis_2d(
    const mg_input_action_map_t* map,
    const char* name,
    float* out_x,
    float* out_y
);

#ifdef __cplusplus
}
#endif

#endif /* MG_INPUT_ACTION_MAP_H */

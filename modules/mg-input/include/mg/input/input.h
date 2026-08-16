/**
 * @file input.h
 * @brief MultiGUIEngine Unified Reactive Input Subsystem.
 */
#ifndef MG_INPUT_INPUT_H
#define MG_INPUT_INPUT_H

#include "mg/pal/window.h"
#include "mg/rx/rx.h"
#include "mg/input/gamepad.h"
#include "mg/input/action_map.h"
#include <stdbool.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_action_type {
    MG_ACTION_BUTTON = 0,
    MG_ACTION_AXIS
} mg_action_type_t;

typedef struct mg_action_event {
    const char* name;
    float value;
    bool pressed;
} mg_action_event_t;

typedef struct mg_input_manager mg_input_manager_t;

/**
 * @brief Creates an input manager.
 */
mg_input_manager_t* mg_input_create(void);

/**
 * @brief Destroys an input manager.
 */
void mg_input_destroy(mg_input_manager_t* input);

/**
 * @brief Binds a physical key code to a named action.
 */
void mg_input_bind_key(mg_input_manager_t* input, const char* action_name, uint32_t key_code);

/**
 * @brief Processes an incoming window event (typically during MG_STAGE_INPUT).
 */
void mg_input_process_event(mg_input_manager_t* input, const mg_event_t* event);

/**
 * @brief Checks if a named action button is currently held down.
 */
bool mg_input_is_down(const mg_input_manager_t* input, const char* action_name);

/**
 * @brief Checks if a named action button was pressed this frame.
 */
bool mg_input_is_pressed(const mg_input_manager_t* input, const char* action_name);

/**
 * @brief Returns the reactive subject stream for action events.
 */
mg_subject_t* mg_input_get_action_subject(mg_input_manager_t* input);

/**
 * @brief Updates state at the end of the frame (clears single-frame press/release flags).
 */
void mg_input_end_frame(mg_input_manager_t* input);

#ifdef __cplusplus
}
#endif

#endif /* MG_INPUT_INPUT_H */

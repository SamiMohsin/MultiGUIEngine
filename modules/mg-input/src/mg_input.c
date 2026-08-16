/**
 * @file mg_input.c
 * @brief MultiGUIEngine Unified Reactive Input Subsystem implementation.
 */
#include "mg/input/input.h"
#include <stdlib.h>
#include <string.h>

#define MG_MAX_INPUT_BINDINGS 128
#define MG_MAX_ACTION_NAME_LEN 32

typedef struct {
    char action_name[MG_MAX_ACTION_NAME_LEN];
    uint32_t key_code;
    bool is_down;
    bool was_pressed;
    bool was_released;
} mg_input_binding_t;

struct mg_input_manager {
    mg_input_binding_t bindings[MG_MAX_INPUT_BINDINGS];
    size_t binding_count;
    mg_subject_t* action_subject;
};

mg_input_manager_t* mg_input_create(void) {
    mg_input_manager_t* input = (mg_input_manager_t*)malloc(sizeof(mg_input_manager_t));
    if (!input) return NULL;

    memset(input, 0, sizeof(mg_input_manager_t));
    input->action_subject = mg_subject_create();
    return input;
}

void mg_input_destroy(mg_input_manager_t* input) {
    if (!input) return;
    if (input->action_subject) {
        mg_subject_destroy(input->action_subject);
    }
    free(input);
}

void mg_input_bind_key(mg_input_manager_t* input, const char* action_name, uint32_t key_code) {
    if (!input || !action_name || input->binding_count >= MG_MAX_INPUT_BINDINGS) return;

    /* Check existing binding */
    for (size_t i = 0; i < input->binding_count; ++i) {
        if (strcmp(input->bindings[i].action_name, action_name) == 0 &&
            input->bindings[i].key_code == key_code) {
            return;
        }
    }

    mg_input_binding_t* b = &input->bindings[input->binding_count++];
    strncpy(b->action_name, action_name, MG_MAX_ACTION_NAME_LEN - 1);
    b->action_name[MG_MAX_ACTION_NAME_LEN - 1] = '\0';
    b->key_code = key_code;
    b->is_down = false;
    b->was_pressed = false;
    b->was_released = false;
}

void mg_input_process_event(mg_input_manager_t* input, const mg_event_t* event) {
    if (!input || !event) return;

    if (event->type == MG_EVENT_KEY_DOWN || event->type == MG_EVENT_KEY_UP) {
        bool is_down = (event->type == MG_EVENT_KEY_DOWN);
        uint32_t key = (uint32_t)event->data.key.key_code;

        for (size_t i = 0; i < input->binding_count; ++i) {
            mg_input_binding_t* b = &input->bindings[i];
            if (b->key_code == key) {
                if (is_down && !b->is_down) {
                    b->was_pressed = true;
                } else if (!is_down && b->is_down) {
                    b->was_released = true;
                }
                b->is_down = is_down;

                /* Emit to reactive stream */
                mg_action_event_t act_ev = {
                    .name = b->action_name,
                    .value = is_down ? 1.0f : 0.0f,
                    .pressed = is_down
                };
                mg_subject_emit(input->action_subject, &act_ev);
            }
        }
    }
}

bool mg_input_is_down(const mg_input_manager_t* input, const char* action_name) {
    if (!input || !action_name) return false;
    for (size_t i = 0; i < input->binding_count; ++i) {
        if (strcmp(input->bindings[i].action_name, action_name) == 0) {
            if (input->bindings[i].is_down) return true;
        }
    }
    return false;
}

bool mg_input_is_pressed(const mg_input_manager_t* input, const char* action_name) {
    if (!input || !action_name) return false;
    for (size_t i = 0; i < input->binding_count; ++i) {
        if (strcmp(input->bindings[i].action_name, action_name) == 0) {
            if (input->bindings[i].was_pressed) return true;
        }
    }
    return false;
}

mg_subject_t* mg_input_get_action_subject(mg_input_manager_t* input) {
    return input ? input->action_subject : NULL;
}

void mg_input_end_frame(mg_input_manager_t* input) {
    if (!input) return;
    for (size_t i = 0; i < input->binding_count; ++i) {
        input->bindings[i].was_pressed = false;
        input->bindings[i].was_released = false;
    }
}

/**
 * @file test_mg_input.c
 * @brief Comprehensive headless unit tests for mg-input.
 */
#include "mg/input/input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_action_rx_count = 0;
static void on_action_event(void* val, void* ctx) {
    (void)ctx;
    const mg_action_event_t* ev = (const mg_action_event_t*)val;
    if (ev && strcmp(ev->name, "jump") == 0 && ev->pressed) {
        g_action_rx_count++;
    }
}

static int test_input_action_mapping(void) {
    printf("Testing input action mapping and reactive streams...\n");

    mg_input_manager_t* input = mg_input_create();
    if (!input) return 1;

    mg_input_bind_key(input, "jump", 32); /* Key 32 (Space) */

    mg_subject_t* act_subj = mg_input_get_action_subject(input);
    mg_subscription_t* sub = mg_subject_subscribe(act_subj, on_action_event, NULL);

    /* Synthetic key down event */
    mg_event_t ev_down = {
        .type = MG_EVENT_KEY_DOWN,
        .data = { .key = { .key_code = 32, .modifiers = 0 } }
    };
    mg_input_process_event(input, &ev_down);

    if (!mg_input_is_down(input, "jump")) {
        fprintf(stderr, "FAIL: jump action should be down\n");
        return 1;
    }
    if (!mg_input_is_pressed(input, "jump")) {
        fprintf(stderr, "FAIL: jump action should be pressed\n");
        return 1;
    }
    if (g_action_rx_count != 1) {
        fprintf(stderr, "FAIL: reactive stream did not receive jump event (count=%d)\n", g_action_rx_count);
        return 1;
    }

    /* End frame */
    mg_input_end_frame(input);
    if (mg_input_is_pressed(input, "jump")) {
        fprintf(stderr, "FAIL: jump action was_pressed should be cleared after end_frame\n");
        return 1;
    }
    if (!mg_input_is_down(input, "jump")) {
        fprintf(stderr, "FAIL: jump action should still be held down\n");
        return 1;
    }

    /* Synthetic key up event */
    mg_event_t ev_up = {
        .type = MG_EVENT_KEY_UP,
        .data = { .key = { .key_code = 32, .modifiers = 0 } }
    };
    mg_input_process_event(input, &ev_up);

    if (mg_input_is_down(input, "jump")) {
        fprintf(stderr, "FAIL: jump action should not be down after key up\n");
        return 1;
    }

    mg_subscription_dispose(sub);
    mg_input_destroy(input);

    printf("PASS: input action mapping and reactive streams\n");
    return 0;
}

int main(void) {
    if (test_input_action_mapping() != 0) return 1;

    printf("All mg-input unit tests passed successfully!\n");
    return 0;
}

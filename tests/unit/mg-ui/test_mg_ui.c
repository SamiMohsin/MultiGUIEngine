/**
 * @file test_mg_ui.c
 * @brief Comprehensive headless unit tests for mg-ui.
 */
#include "mg/ui/ui.h"
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>

static int test_ui_button_interaction(void) {
    printf("Testing UI button interaction and hit-testing...\n");

    mg_ui_context_t* ui = mg_ui_create(NULL);
    if (!ui) return 1;

    /* Move mouse over button at (100, 100, 200, 50) and click */
    mg_event_t ev_move = {
        .type = MG_EVENT_MOUSE_MOVE,
        .data = { .mouse_move = { .x = 150.0f, .y = 120.0f, .dx = 0.0f, .dy = 0.0f } }
    };
    mg_ui_process_event(ui, &ev_move);

    mg_event_t ev_click = {
        .type = MG_EVENT_MOUSE_DOWN,
        .data = { .mouse_button = { .button = 0, .x = 150.0f, .y = 120.0f } }
    };
    mg_ui_process_event(ui, &ev_click);

    mg_ui_begin(ui, 800, 600);
    bool clicked = mg_ui_button(ui, "Click Me", 100, 100, 200, 50);
    if (!clicked) {
        fprintf(stderr, "FAIL: button within mouse bounds was not clicked\n");
        mg_ui_destroy(ui);
        return 1;
    }

    bool clicked_other = mg_ui_button(ui, "Other", 400, 400, 100, 50);
    if (clicked_other) {
        fprintf(stderr, "FAIL: other button outside mouse bounds was clicked\n");
        mg_ui_destroy(ui);
        return 1;
    }

    mg_ui_destroy(ui);
    printf("PASS: UI button interaction and hit-testing\n");
    return 0;
}

int main(void) {
    if (test_ui_button_interaction() != 0) return 1;

    printf("All mg-ui unit tests passed successfully!\n");
    return 0;
}

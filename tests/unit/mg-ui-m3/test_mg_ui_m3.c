/**
 * @file test_mg_ui_m3.c
 * @brief Unit tests for Google Material Design 3 UI component system.
 */
#include "mg/ui/ui.h"
#include "mg/pal/cpu.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static void test_cpu_caps(void) {
    mg_cpu_caps_t caps = mg_cpu_get_capabilities();
    assert(caps.logical_cores >= 1);
    assert(caps.arch != MG_ARCH_UNKNOWN);
    assert(caps.arch_name != NULL);
    printf("PASS: test_cpu_caps (%s)\n", caps.arch_name);
}

static void test_m3_themes(void) {
    mg_m3_color_scheme_t dark = mg_m3_theme_dark();
    assert(dark.primary.a == 1.0f);
    assert(dark.surface.r < 0.2f); /* Dark surface */

    mg_m3_color_scheme_t light = mg_m3_theme_light();
    assert(light.primary.a == 1.0f);
    assert(light.surface.r > 0.8f); /* Light surface */
    printf("PASS: test_m3_themes\n");
}

static void test_m3_widgets(void) {
    mg_ui_context_t* ui = mg_ui_create(NULL);
    assert(ui != NULL);

    mg_ui_begin(ui, 1280, 720);

    /* Buttons */
    bool b1 = mg_ui_m3_button(ui, "Test", MG_ICON_ADD, MG_M3_BTN_FILLED, 10, 10, 100, 40);
    assert(!b1);
    bool b2 = mg_ui_m3_icon_button(ui, MG_ICON_FAVORITE, false, 120, 10, 40);
    assert(!b2);
    bool b3 = mg_ui_m3_fab(ui, MG_ICON_ADD, 170, 10, 48);
    assert(!b3);

    /* Selection controls */
    bool chk = false;
    mg_ui_m3_checkbox(ui, "Check", &chk, 10, 60);
    bool rad = mg_ui_m3_radio_button(ui, "Radio", true, 100, 60);
    assert(!rad);
    bool sw = false;
    mg_ui_m3_switch(ui, "Switch", &sw, 200, 60);

    /* Sliders */
    float val = 50.0f;
    mg_ui_m3_slider(ui, &val, 0.0f, 100.0f, 10, 100, 200);

    /* Pickers */
    int yr = 2026, mo = 8, dy = 16;
    mg_ui_m3_date_picker(ui, &yr, &mo, &dy, 10, 140, 200, 100);

    /* Navigation */
    const char* tabs[2] = { "Tab1", "Tab2" };
    int active_tab = mg_ui_m3_tabs(ui, tabs, 2, 0, 10, 260, 300);
    assert(active_tab == 0);

    /* Dialog */
    bool dlg = mg_ui_m3_dialog(ui, "Alert", "Body text", "OK", "Cancel", 1280, 720);
    assert(!dlg);

    /* Progress & Chips */
    mg_ui_m3_progress_linear(ui, 0.5f, 10, 320, 200, 4);
    mg_ui_m3_progress_circular(ui, 0.5f, 50, 350, 16);
    mg_ui_m3_chip(ui, "Chip", MG_ICON_NONE, MG_M3_CHIP_ASSIST, false, 10, 380);

    mg_ui_destroy(ui);
    printf("PASS: test_m3_widgets\n");
}

int main(void) {
    printf("=== MultiGUIEngine Material 3 UI Unit Tests ===\n");
    test_cpu_caps();
    test_m3_themes();
    test_m3_widgets();
    printf("All Material 3 UI tests passed successfully.\n");
    return 0;
}

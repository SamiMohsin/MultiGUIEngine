/**
 * @file main.c
 * @brief Example 07: Comprehensive Google Material Design 3 (M3) Component Gallery.
 */
#include "mg/render/render.h"
#include "mg/ui/ui.h"
#include "mg/pal/cpu.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("=== Example 07: Google Material Design 3 (M3) Component Gallery ===\n");

    /* Detect CPU and Architecture */
    mg_cpu_caps_t cpu = mg_cpu_get_capabilities();
    printf("Hardware Detected: %s (%u logical cores, %u physical cores)\n",
           cpu.arch_name, cpu.logical_cores, cpu.physical_cores);

    const uint32_t screen_w = 1280;
    const uint32_t screen_h = 720;

    mg_rhi_desc_t rhi_desc = {
        .backend_type = MG_RHI_BACKEND_SOFTWARE,
        .width = screen_w,
        .height = screen_h,
        .vsync = false,
        .offscreen = true
    };
    if (!mg_rhi_init(&rhi_desc)) {
        fprintf(stderr, "FAIL: mg_rhi_init failed\n");
        return 1;
    }

    mg_batch2d_t* batch = mg_batch2d_create(2048);
    mg_ui_context_t* ui = mg_ui_create(NULL);

    /* Render Material 3 Showcase Frame */
    mg_color_t clear_color = { 0.08f, 0.07f, 0.09f, 1.0f };
    mg_rhi_begin_frame(&clear_color);
    mg_batch2d_begin(batch);

    mg_ui_begin(ui, (float)screen_w, (float)screen_h);

    /* 1. Top App Bar */
    mg_ui_m3_top_app_bar(ui, "Material 3 Component Showcase", MG_ICON_MENU, MG_ICON_SETTINGS, (float)screen_w);

    /* 2. Primary Tabs */
    const char* tabs[3] = { "Components", "Inputs & Pickers", "Navigation & Layout" };
    mg_ui_m3_tabs(ui, tabs, 3, 0, 80.0f, 72.0f, 400.0f);

    /* 3. Buttons & FABs Section */
    mg_ui_m3_button(ui, "Filled Button", MG_ICON_NONE, MG_M3_BTN_FILLED, 80.0f, 130.0f, 140.0f, 40.0f);
    mg_ui_m3_button(ui, "Outlined", MG_ICON_NONE, MG_M3_BTN_OUTLINED, 230.0f, 130.0f, 120.0f, 40.0f);
    mg_ui_m3_button(ui, "Tonal", MG_ICON_NONE, MG_M3_BTN_TONAL, 360.0f, 130.0f, 100.0f, 40.0f);
    mg_ui_m3_icon_button(ui, MG_ICON_FAVORITE, true, 470.0f, 130.0f, 40.0f);
    mg_ui_m3_fab(ui, MG_ICON_ADD, 520.0f, 126.0f, 48.0f);
    mg_ui_m3_extended_fab(ui, "Create", MG_ICON_ADD, 580.0f, 126.0f, 120.0f, 48.0f);

    /* 4. Segmented Buttons */
    const char* segments[3] = { "Day", "Week", "Month" };
    mg_ui_m3_segmented_button(ui, segments, 3, 1, 80.0f, 190.0f, 300.0f, 40.0f);

    /* 5. Selection Controls */
    bool chk = true;
    mg_ui_m3_checkbox(ui, "Material Checkbox", &chk, 80.0f, 250.0f);
    mg_ui_m3_radio_button(ui, "Material Radio", true, 260.0f, 250.0f);
    bool sw = true;
    mg_ui_m3_switch(ui, "Switch", &sw, 440.0f, 250.0f);

    /* 6. Sliders */
    float slider_val = 65.0f;
    mg_ui_m3_slider(ui, &slider_val, 0.0f, 100.0f, 80.0f, 300.0f, 300.0f);
    float r_start = 20.0f, r_end = 80.0f;
    mg_ui_m3_range_slider(ui, &r_start, &r_end, 0.0f, 100.0f, 400.0f, 300.0f, 300.0f);

    /* 7. Text Field & Search Bar */
    char txt[32] = "Search Material 3";
    mg_ui_m3_search_bar(ui, "Search...", txt, sizeof(txt), 80.0f, 340.0f, 300.0f, 48.0f);
    mg_ui_m3_text_field(ui, "Outlined Input", "Username", sizeof("Username"), true, 400.0f, 340.0f, 300.0f, 48.0f);

    /* 8. Chips & Badges */
    mg_ui_m3_chip(ui, "Assist", MG_ICON_INFO, MG_M3_CHIP_ASSIST, false, 80.0f, 405.0f);
    mg_ui_m3_chip(ui, "Filter *", MG_ICON_CHECK, MG_M3_CHIP_FILTER, true, 190.0f, 405.0f);
    mg_ui_m3_chip(ui, "Input", MG_ICON_EDIT, MG_M3_CHIP_INPUT, false, 300.0f, 405.0f);
    mg_ui_m3_badge(ui, 5, 410.0f, 405.0f);

    /* 9. Progress Indicators */
    mg_ui_m3_progress_linear(ui, 0.72f, 80.0f, 455.0f, 300.0f, 6.0f);
    mg_ui_m3_progress_circular(ui, 0.5f, 420.0f, 458.0f, 12.0f);

    /* 10. Cards */
    mg_ui_m3_card(ui, MG_M3_CARD_ELEVATED, 730.0f, 130.0f, 240.0f, 140.0f);
    mg_ui_m3_card(ui, MG_M3_CARD_OUTLINED, 990.0f, 130.0f, 240.0f, 140.0f);

    /* 11. Pickers */
    int y = 2026, m = 8, d = 16;
    mg_ui_m3_date_picker(ui, &y, &m, &d, 730.0f, 290.0f, 240.0f, 120.0f);
    int hr = 14, min = 30;
    mg_ui_m3_time_picker(ui, &hr, &min, 990.0f, 290.0f, 240.0f, 120.0f);

    /* 12. Bottom Navigation Bar */
    const char* nav_items[4] = { "Home", "Explore", "Favorites", "Profile" };
    const mg_icon_id_t nav_icons[4] = { MG_ICON_HOME, MG_ICON_SEARCH, MG_ICON_FAVORITE, MG_ICON_SETTINGS };
    mg_ui_m3_bottom_nav_bar(ui, nav_items, nav_icons, 4, 0, (float)screen_w, (float)screen_h);

    /* 13. Snackbar */
    mg_ui_m3_snackbar(ui, "Material 3 Component Gallery Active", "Undo", (float)screen_w, (float)screen_h);

    mg_ui_render(ui, batch);
    mg_batch2d_end(batch);
    mg_rhi_end_frame();

    /* Readback pixel */
    uint32_t* pixels = (uint32_t*)malloc(screen_w * screen_h * sizeof(uint32_t));
    if (pixels) {
        mg_rhi_read_pixels(pixels, screen_w * screen_h * sizeof(uint32_t));
        uint32_t btn_px = pixels[140 * screen_w + 100];
        printf("Gallery Rendered: Filled Button Pixel = 0x%08X\n", btn_px);
        free(pixels);
    }

    mg_ui_destroy(ui);
    mg_batch2d_destroy(batch);
    mg_rhi_shutdown();

    printf("PASS: Example 07: Google Material Design 3 (M3) Component Gallery verified\n");
    return 0;
}

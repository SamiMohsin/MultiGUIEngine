/**
 * @file main.c
 * @brief Example 15: Material Design 3 GUI Dashboard with Icons, Typography Fonts, Art & Dynamic Themes.
 */
#include "mg/render/render.h"
#include "mg/ui/ui.h"
#include "mg/pal/pal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

static void build_dashboard_ui(mg_ui_context_t* ui) {
    /* 1. Top App Bar with Icons */
    mg_ui_m3_top_app_bar(ui, "MultiGUI Studio — Creative Hub", MG_ICON_MENU, MG_ICON_SETTINGS, (float)WINDOW_WIDTH);

    /* 2. Left Navigation Rail */
    const mg_icon_id_t nav_icons[4] = { MG_ICON_HOME, MG_ICON_SEARCH, MG_ICON_STAR, MG_ICON_FAVORITE };
    const char* nav_labels[4] = { "Home", "Explore", "Saved", "Likes" };
    mg_ui_m3_navigation_rail(ui, nav_labels, nav_icons, 4, 0, (float)WINDOW_HEIGHT - 64.0f);

    /* 3. Hero Featured Art & Engine Stat Card */
    mg_ui_m3_card(ui, MG_M3_CARD_ELEVATED, 100.0f, 80.0f, 560.0f, 220.0f);
    mg_ui_m3_button(ui, "Launch Project", MG_ICON_NONE, MG_M3_BTN_FILLED, 120.0f, 240.0f, 150.0f, 40.0f);
    mg_ui_m3_button(ui, "Configure", MG_ICON_SETTINGS, MG_M3_BTN_OUTLINED, 290.0f, 240.0f, 130.0f, 40.0f);

    /* 4. Right Status Column */
    mg_ui_m3_card(ui, MG_M3_CARD_FILLED, 680.0f, 80.0f, 500.0f, 100.0f);
    mg_ui_m3_badge(ui, 144, 1100.0f, 95.0f);

    mg_ui_m3_card(ui, MG_M3_CARD_FILLED, 680.0f, 200.0f, 500.0f, 100.0f);
    mg_ui_m3_badge(ui, 5, 1100.0f, 215.0f);

    /* 5. Lower Component Section */
    const char* time_filters[3] = { "Real-Time", "Session", "Analytics" };
    mg_ui_m3_segmented_button(ui, time_filters, 3, 0, 100.0f, 320.0f, 360.0f, 40.0f);

    float slider_vol = 0.85f;
    mg_ui_m3_slider(ui, &slider_vol, 0.0f, 1.0f, 100.0f, 380.0f, 360.0f);

    /* Action Buttons & Badges */
    mg_ui_m3_button(ui, "Export Asset .mgpak", MG_ICON_SHARE, MG_M3_BTN_TONAL, 480.0f, 320.0f, 180.0f, 40.0f);
    mg_ui_m3_icon_button(ui, MG_ICON_REFRESH, true, 680.0f, 320.0f, 40.0f);
    mg_ui_m3_icon_button(ui, MG_ICON_FAVORITE, false, 730.0f, 320.0f, 40.0f);

    /* Floating Action Button (FAB) */
    mg_ui_m3_fab(ui, MG_ICON_ADD, (float)WINDOW_WIDTH - 90.0f, (float)WINDOW_HEIGHT - 90.0f, 56.0f);

    /* Bottom Snackbar */
    mg_ui_m3_snackbar(ui, "Theme switched to Material 3 Palette", "DISMISS", (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
}

int main(void) {
    printf("=== Example 15: Material 3 GUI Dashboard with Icons, Typography Fonts & Themes ===\n");

    /* 1. Initialize RHI Software Backend */
    mg_rhi_desc_t rhi_desc = {
        .backend_type = MG_RHI_BACKEND_SOFTWARE,
        .width = WINDOW_WIDTH,
        .height = WINDOW_HEIGHT,
        .vsync = false,
        .offscreen = true
    };

    if (!mg_rhi_init(&rhi_desc)) {
        fprintf(stderr, "FAIL: mg_rhi_init failed\n");
        return 1;
    }

    mg_batch2d_t* batch = mg_batch2d_create(2048);
    mg_ui_context_t* ui = mg_ui_create(NULL);
    assert(batch != NULL && ui != NULL);

    uint32_t* pixels = (uint32_t*)malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
    assert(pixels != NULL);

    /* 2. Render Pass A: Material 3 Dark Theme */
    mg_m3_color_scheme_t dark_theme = mg_m3_theme_dark();
    mg_rhi_begin_frame(&dark_theme.surface);
    mg_batch2d_begin(batch);

    /* Render Custom Header Font Typography & Art */
    mg_color_t header_color = dark_theme.primary;
    mg_font_draw_text(batch, "VULKAN RHI & REAL-TIME DSP PIPELINE\nNext-Gen C99 Engine", 120.0f, 100.0f, 1.2f, &header_color);

    mg_color_t stat_color = dark_theme.on_surface;
    mg_font_draw_text(batch, "Performance: 144 FPS (~3.3 us Frame Latency)", 700.0f, 100.0f, 1.0f, &stat_color);
    mg_font_draw_text(batch, "Audio DSP: LowPass 1.0 kHz | Delay 250ms", 700.0f, 220.0f, 1.0f, &stat_color);

    mg_ui_begin(ui, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    build_dashboard_ui(ui);
    mg_ui_render(ui, batch);

    mg_batch2d_end(batch);
    mg_rhi_end_frame();

    mg_rhi_read_pixels(pixels, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
    uint32_t dark_pixel = pixels[WINDOW_HEIGHT / 2 * WINDOW_WIDTH + WINDOW_WIDTH / 2];
    printf("PASS: Material 3 Dark Theme Pass Rendered (Sample Pixel = 0x%08X)\n", dark_pixel);

    /* 3. Render Pass B: Material 3 Light Theme */
    mg_m3_color_scheme_t light_theme = mg_m3_theme_light();
    mg_rhi_begin_frame(&light_theme.surface);
    mg_batch2d_begin(batch);

    /* Render Custom Light Typography */
    mg_color_t light_header_color = light_theme.primary;
    mg_font_draw_text(batch, "VULKAN RHI & REAL-TIME DSP PIPELINE\nNext-Gen C99 Engine", 120.0f, 100.0f, 1.2f, &light_header_color);

    mg_ui_begin(ui, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    build_dashboard_ui(ui);
    mg_ui_render(ui, batch);

    mg_batch2d_end(batch);
    mg_rhi_end_frame();

    mg_rhi_read_pixels(pixels, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
    uint32_t light_pixel = pixels[WINDOW_HEIGHT / 2 * WINDOW_WIDTH + WINDOW_WIDTH / 2];
    printf("PASS: Material 3 Light Theme Pass Rendered (Sample Pixel = 0x%08X)\n", light_pixel);

    /* Cleanup */
    free(pixels);
    mg_ui_destroy(ui);
    mg_batch2d_destroy(batch);
    mg_rhi_shutdown();

    printf("PASS: Example 15: Material 3 GUI Dashboard with Icons, Fonts & Themes completed successfully\n");
    return 0;
}

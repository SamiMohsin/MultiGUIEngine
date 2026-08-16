/**
 * @file material3.h
 * @brief Google Material Design 3 (M3) Complete Component Library API.
 */
#ifndef MG_UI_MATERIAL3_H
#define MG_UI_MATERIAL3_H

#include "mg/ui/theme.h"
#include "mg/ui/icons.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_ui_context mg_ui_context_t;


typedef enum mg_m3_button_type {
    MG_M3_BTN_FILLED = 0,
    MG_M3_BTN_OUTLINED,
    MG_M3_BTN_TEXT,
    MG_M3_BTN_ELEVATED,
    MG_M3_BTN_TONAL
} mg_m3_button_type_t;

typedef enum mg_m3_chip_type {
    MG_M3_CHIP_ASSIST = 0,
    MG_M3_CHIP_FILTER,
    MG_M3_CHIP_INPUT,
    MG_M3_CHIP_SUGGESTION
} mg_m3_chip_type_t;

typedef enum mg_m3_card_type {
    MG_M3_CARD_ELEVATED = 0,
    MG_M3_CARD_FILLED,
    MG_M3_CARD_OUTLINED
} mg_m3_card_type_t;

/* --- Buttons & Action Elements --- */

bool mg_ui_m3_button(
    mg_ui_context_t* ctx,
    const char* label,
    mg_icon_id_t icon,
    mg_m3_button_type_t type,
    float x, float y, float width, float height
);

bool mg_ui_m3_icon_button(
    mg_ui_context_t* ctx,
    mg_icon_id_t icon,
    bool filled,
    float x, float y, float size
);

bool mg_ui_m3_fab(
    mg_ui_context_t* ctx,
    mg_icon_id_t icon,
    float x, float y, float size
);

bool mg_ui_m3_extended_fab(
    mg_ui_context_t* ctx,
    const char* label,
    mg_icon_id_t icon,
    float x, float y, float width, float height
);

int mg_ui_m3_segmented_button(
    mg_ui_context_t* ctx,
    const char** segment_labels,
    uint32_t count,
    int selected_index,
    float x, float y, float width, float height
);

/* --- Selection Controls & Sliders --- */

bool mg_ui_m3_checkbox(
    mg_ui_context_t* ctx,
    const char* label,
    bool* checked,
    float x, float y
);

bool mg_ui_m3_radio_button(
    mg_ui_context_t* ctx,
    const char* label,
    bool is_selected,
    float x, float y
);

bool mg_ui_m3_switch(
    mg_ui_context_t* ctx,
    const char* label,
    bool* active,
    float x, float y
);

bool mg_ui_m3_slider(
    mg_ui_context_t* ctx,
    float* value,
    float min_val,
    float max_val,
    float x, float y, float width
);

bool mg_ui_m3_range_slider(
    mg_ui_context_t* ctx,
    float* start_val,
    float* end_val,
    float min_val,
    float max_val,
    float x, float y, float width
);

/* --- Inputs & Pickers --- */

bool mg_ui_m3_text_field(
    mg_ui_context_t* ctx,
    const char* label,
    char* text_buffer,
    size_t buffer_size,
    bool outlined,
    float x, float y, float width, float height
);

bool mg_ui_m3_search_bar(
    mg_ui_context_t* ctx,
    const char* placeholder,
    char* text_buffer,
    size_t buffer_size,
    float x, float y, float width, float height
);

bool mg_ui_m3_date_picker(
    mg_ui_context_t* ctx,
    int* year, int* month, int* day,
    float x, float y, float width, float height
);

bool mg_ui_m3_time_picker(
    mg_ui_context_t* ctx,
    int* hour, int* minute,
    float x, float y, float width, float height
);

/* --- Navigation & App Bars --- */

void mg_ui_m3_top_app_bar(
    mg_ui_context_t* ctx,
    const char* title,
    mg_icon_id_t nav_icon,
    mg_icon_id_t action_icon,
    float screen_width
);

int mg_ui_m3_bottom_nav_bar(
    mg_ui_context_t* ctx,
    const char** item_labels,
    const mg_icon_id_t* item_icons,
    uint32_t count,
    int selected_index,
    float screen_width, float screen_height
);

int mg_ui_m3_navigation_rail(
    mg_ui_context_t* ctx,
    const char** item_labels,
    const mg_icon_id_t* item_icons,
    uint32_t count,
    int selected_index,
    float screen_height
);

int mg_ui_m3_tabs(
    mg_ui_context_t* ctx,
    const char** tab_labels,
    uint32_t count,
    int active_tab,
    float x, float y, float width
);

/* --- Sheets, Dialogs & Overlays --- */

bool mg_ui_m3_dialog(
    mg_ui_context_t* ctx,
    const char* title,
    const char* message,
    const char* confirm_label,
    const char* dismiss_label,
    float screen_width, float screen_height
);

void mg_ui_m3_bottom_sheet(
    mg_ui_context_t* ctx,
    float screen_width, float screen_height,
    float height,
    const mg_color_t* surface_color
);

void mg_ui_m3_side_sheet(
    mg_ui_context_t* ctx,
    float screen_width, float screen_height,
    float width,
    bool is_right_side
);

void mg_ui_m3_snackbar(
    mg_ui_context_t* ctx,
    const char* message,
    const char* action_label,
    float screen_width, float screen_height
);

void mg_ui_m3_tooltip(
    mg_ui_context_t* ctx,
    const char* text,
    float target_x, float target_y
);

/* --- Data, Feedback & Cards --- */

void mg_ui_m3_card(
    mg_ui_context_t* ctx,
    mg_m3_card_type_t type,
    float x, float y, float width, float height
);

bool mg_ui_m3_chip(
    mg_ui_context_t* ctx,
    const char* label,
    mg_icon_id_t icon,
    mg_m3_chip_type_t type,
    bool selected,
    float x, float y
);

void mg_ui_m3_badge(
    mg_ui_context_t* ctx,
    int count,
    float x, float y
);

void mg_ui_m3_progress_linear(
    mg_ui_context_t* ctx,
    float progress, /* [0.0, 1.0] */
    float x, float y, float width, float height
);

void mg_ui_m3_progress_circular(
    mg_ui_context_t* ctx,
    float progress, /* [0.0, 1.0] */
    float center_x, float center_y, float radius
);

void mg_ui_m3_divider(
    mg_ui_context_t* ctx,
    float x, float y, float width
);

bool mg_ui_m3_list_item(
    mg_ui_context_t* ctx,
    const char* headline,
    const char* supporting_text,
    mg_icon_id_t leading_icon,
    mg_icon_id_t trailing_icon,
    float x, float y, float width, float height
);

void mg_ui_m3_carousel(
    mg_ui_context_t* ctx,
    uint32_t item_count,
    int current_index,
    float x, float y, float width, float height
);

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_MATERIAL3_H */

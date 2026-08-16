#include "mg/ui/ui.h"
#include "mg/ui/material3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


mg_m3_color_scheme_t mg_m3_theme_dark(void) {
    mg_m3_color_scheme_t s;
    s.primary = (mg_color_t){ 0.82f, 0.75f, 1.0f, 1.0f };                 /* #D0BCFF */
    s.on_primary = (mg_color_t){ 0.24f, 0.16f, 0.44f, 1.0f };              /* #381E72 */
    s.primary_container = (mg_color_t){ 0.31f, 0.24f, 0.52f, 1.0f };       /* #4F378B */
    s.on_primary_container = (mg_color_t){ 0.91f, 0.87f, 1.0f, 1.0f };     /* #EADDFF */

    s.secondary = (mg_color_t){ 0.80f, 0.77f, 0.86f, 1.0f };               /* #CCC2DC */
    s.on_secondary = (mg_color_t){ 0.20f, 0.18f, 0.26f, 1.0f };             /* #332D41 */
    s.secondary_container = (mg_color_t){ 0.29f, 0.26f, 0.35f, 1.0f };      /* #4A4458 */
    s.on_secondary_container = (mg_color_t){ 0.91f, 0.88f, 0.96f, 1.0f };    /* #E8DEF8 */

    s.tertiary = (mg_color_t){ 0.94f, 0.72f, 0.79f, 1.0f };                /* #EFB8C8 */
    s.on_tertiary = (mg_color_t){ 0.31f, 0.11f, 0.18f, 1.0f };              /* #492532 */
    s.tertiary_container = (mg_color_t){ 0.44f, 0.21f, 0.30f, 1.0f };       /* #705562 */
    s.on_tertiary_container = (mg_color_t){ 1.0f, 0.85f, 0.90f, 1.0f };     /* #FFD8E4 */

    s.error = (mg_color_t){ 0.95f, 0.54f, 0.52f, 1.0f };                   /* #F2B8B5 */
    s.on_error = (mg_color_t){ 0.38f, 0.09f, 0.07f, 1.0f };                 /* #601410 */
    s.error_container = (mg_color_t){ 0.55f, 0.16f, 0.15f, 1.0f };          /* #8C1D18 */
    s.on_error_container = (mg_color_t){ 0.98f, 0.87f, 0.86f, 1.0f };        /* #F9DEDC */

    s.surface = (mg_color_t){ 0.08f, 0.07f, 0.09f, 1.0f };                 /* #141218 */
    s.on_surface = (mg_color_t){ 0.90f, 0.89f, 0.92f, 1.0f };              /* #E6E0E9 */
    s.surface_variant = (mg_color_t){ 0.31f, 0.29f, 0.33f, 1.0f };         /* #49454F */
    s.on_surface_variant = (mg_color_t){ 0.80f, 0.77f, 0.82f, 1.0f };      /* #CAC4D0 */
    s.surface_container = (mg_color_t){ 0.13f, 0.12f, 0.15f, 1.0f };       /* #211F26 */
    s.surface_container_high = (mg_color_t){ 0.16f, 0.15f, 0.18f, 1.0f };  /* #2B2930 */
    s.surface_container_highest = (mg_color_t){ 0.21f, 0.20f, 0.23f, 1.0f };/* #36343B */

    s.outline = (mg_color_t){ 0.58f, 0.55f, 0.60f, 1.0f };                 /* #938F99 */
    s.outline_variant = (mg_color_t){ 0.31f, 0.29f, 0.33f, 1.0f };         /* #49454F */
    s.scrim = (mg_color_t){ 0.0f, 0.0f, 0.0f, 0.6f };
    s.shadow = (mg_color_t){ 0.0f, 0.0f, 0.0f, 0.3f };
    return s;
}

mg_m3_color_scheme_t mg_m3_theme_light(void) {
    mg_m3_color_scheme_t s;
    s.primary = (mg_color_t){ 0.40f, 0.31f, 0.64f, 1.0f };                 /* #6750A4 */
    s.on_primary = (mg_color_t){ 1.0f, 1.0f, 1.0f, 1.0f };
    s.primary_container = (mg_color_t){ 0.91f, 0.87f, 1.0f, 1.0f };
    s.on_primary_container = (mg_color_t){ 0.13f, 0.03f, 0.35f, 1.0f };

    s.secondary = (mg_color_t){ 0.38f, 0.36f, 0.44f, 1.0f };
    s.on_secondary = (mg_color_t){ 1.0f, 1.0f, 1.0f, 1.0f };
    s.secondary_container = (mg_color_t){ 0.91f, 0.88f, 0.96f, 1.0f };
    s.on_secondary_container = (mg_color_t){ 0.11f, 0.09f, 0.16f, 1.0f };

    s.tertiary = (mg_color_t){ 0.48f, 0.32f, 0.39f, 1.0f };
    s.on_tertiary = (mg_color_t){ 1.0f, 1.0f, 1.0f, 1.0f };
    s.tertiary_container = (mg_color_t){ 1.0f, 0.85f, 0.90f, 1.0f };
    s.on_tertiary_container = (mg_color_t){ 0.19f, 0.06f, 0.13f, 1.0f };

    s.error = (mg_color_t){ 0.70f, 0.14f, 0.14f, 1.0f };
    s.on_error = (mg_color_t){ 1.0f, 1.0f, 1.0f, 1.0f };
    s.error_container = (mg_color_t){ 0.98f, 0.87f, 0.86f, 1.0f };
    s.on_error_container = (mg_color_t){ 0.25f, 0.03f, 0.03f, 1.0f };

    s.surface = (mg_color_t){ 0.99f, 0.98f, 1.0f, 1.0f };
    s.on_surface = (mg_color_t){ 0.11f, 0.11f, 0.13f, 1.0f };
    s.surface_variant = (mg_color_t){ 0.91f, 0.89f, 0.94f, 1.0f };
    s.on_surface_variant = (mg_color_t){ 0.29f, 0.27f, 0.31f, 1.0f };
    s.surface_container = (mg_color_t){ 0.95f, 0.93f, 0.97f, 1.0f };
    s.surface_container_high = (mg_color_t){ 0.92f, 0.90f, 0.95f, 1.0f };
    s.surface_container_highest = (mg_color_t){ 0.89f, 0.87f, 0.92f, 1.0f };

    s.outline = (mg_color_t){ 0.48f, 0.46f, 0.50f, 1.0f };
    s.outline_variant = (mg_color_t){ 0.80f, 0.77f, 0.82f, 1.0f };
    s.scrim = (mg_color_t){ 0.0f, 0.0f, 0.0f, 0.4f };
    s.shadow = (mg_color_t){ 0.0f, 0.0f, 0.0f, 0.15f };
    return s;
}

static mg_m3_color_scheme_t g_active_theme;
static bool g_theme_initialized = false;

static inline const mg_m3_color_scheme_t* get_theme(void) {
    if (!g_theme_initialized) {
        g_active_theme = mg_m3_theme_dark();
        g_theme_initialized = true;
    }
    return &g_active_theme;
}

bool mg_ui_m3_button(
    mg_ui_context_t* ctx,
    const char* label,
    mg_icon_id_t icon,
    mg_m3_button_type_t type,
    float x, float y, float width, float height
) {
    (void)icon;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_color_t bg = (type == MG_M3_BTN_FILLED) ? t->primary :
                    (type == MG_M3_BTN_TONAL) ? t->secondary_container :
                    (type == MG_M3_BTN_ELEVATED) ? t->surface_container_high :
                    (type == MG_M3_BTN_OUTLINED) ? (mg_color_t){0, 0, 0, 0} : (mg_color_t){0, 0, 0, 0};

    if (type == MG_M3_BTN_OUTLINED) {
        mg_ui_panel(ctx, x, y, width, height, &t->outline);
        mg_ui_panel(ctx, x + 1.0f, y + 1.0f, width - 2.0f, height - 2.0f, &t->surface);
    } else {
        mg_ui_panel(ctx, x, y, width, height, &bg);
    }

    return mg_ui_button(ctx, label, x, y, width, height);
}

bool mg_ui_m3_icon_button(
    mg_ui_context_t* ctx,
    mg_icon_id_t icon,
    bool filled,
    float x, float y, float size
) {
    (void)icon;
    const mg_m3_color_scheme_t* t = get_theme();
    if (filled) {
        mg_ui_panel(ctx, x, y, size, size, &t->primary);
    }
    return mg_ui_button(ctx, "*", x, y, size, size);
}

bool mg_ui_m3_fab(
    mg_ui_context_t* ctx,
    mg_icon_id_t icon,
    float x, float y, float size
) {
    (void)icon;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, size, size, &t->primary_container);
    return mg_ui_button(ctx, "+", x, y, size, size);
}

bool mg_ui_m3_extended_fab(
    mg_ui_context_t* ctx,
    const char* label,
    mg_icon_id_t icon,
    float x, float y, float width, float height
) {
    (void)icon;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, height, &t->primary_container);
    return mg_ui_button(ctx, label, x, y, width, height);
}

int mg_ui_m3_segmented_button(
    mg_ui_context_t* ctx,
    const char** segment_labels,
    uint32_t count,
    int selected_index,
    float x, float y, float width, float height
) {
    if (!segment_labels || count == 0) return selected_index;
    float seg_w = width / (float)count;
    const mg_m3_color_scheme_t* t = get_theme();

    for (uint32_t i = 0; i < count; ++i) {
        float sx = x + (float)i * seg_w;
        mg_color_t col = (selected_index == (int)i) ? t->secondary_container : t->surface_container;
        mg_ui_panel(ctx, sx, y, seg_w, height, &col);

        if (mg_ui_button(ctx, segment_labels[i], sx, y, seg_w, height)) {
            selected_index = (int)i;
        }
    }
    return selected_index;
}

bool mg_ui_m3_checkbox(
    mg_ui_context_t* ctx,
    const char* label,
    bool* checked,
    float x, float y
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float box_size = 20.0f;
    mg_color_t box_col = (checked && *checked) ? t->primary : t->surface_container_highest;
    mg_ui_panel(ctx, x, y, box_size, box_size, &box_col);

    bool clicked = mg_ui_button(ctx, (checked && *checked) ? "✓" : "", x, y, box_size, box_size);
    if (clicked && checked) {
        *checked = !(*checked);
    }
    if (label) {
        mg_ui_button(ctx, label, x + box_size + 8.0f, y, 120.0f, box_size);
    }
    return clicked;
}

bool mg_ui_m3_radio_button(
    mg_ui_context_t* ctx,
    const char* label,
    bool is_selected,
    float x, float y
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float size = 20.0f;
    mg_color_t col = is_selected ? t->primary : t->outline;
    mg_ui_panel(ctx, x, y, size, size, &col);
    if (is_selected) {
        mg_ui_panel(ctx, x + 4.0f, y + 4.0f, size - 8.0f, size - 8.0f, &t->surface);
    }
    bool clicked = mg_ui_button(ctx, is_selected ? "●" : "○", x, y, size, size);
    if (label) {
        mg_ui_button(ctx, label, x + size + 8.0f, y, 120.0f, size);
    }
    return clicked;
}

bool mg_ui_m3_switch(
    mg_ui_context_t* ctx,
    const char* label,
    bool* active,
    float x, float y
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float w = 48.0f, h = 24.0f;
    bool is_on = active && *active;
    mg_color_t track_col = is_on ? t->primary : t->surface_container_highest;
    mg_ui_panel(ctx, x, y, w, h, &track_col);

    float thumb_x = is_on ? (x + w - 20.0f) : (x + 4.0f);
    mg_color_t thumb_col = is_on ? t->on_primary : t->outline;
    mg_ui_panel(ctx, thumb_x, y + 4.0f, 16.0f, 16.0f, &thumb_col);

    bool clicked = mg_ui_button(ctx, "", x, y, w, h);
    if (clicked && active) {
        *active = !(*active);
    }
    if (label) {
        mg_ui_button(ctx, label, x + w + 8.0f, y, 120.0f, h);
    }
    return clicked;
}

bool mg_ui_m3_slider(
    mg_ui_context_t* ctx,
    float* value,
    float min_val,
    float max_val,
    float x, float y, float width
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float h = 16.0f;
    mg_ui_panel(ctx, x, y + 6.0f, width, 4.0f, &t->surface_container_highest);

    float norm = (value && max_val > min_val) ? (*value - min_val) / (max_val - min_val) : 0.5f;
    norm = fmaxf(0.0f, fminf(1.0f, norm));

    mg_ui_panel(ctx, x, y + 6.0f, width * norm, 4.0f, &t->primary);

    float thumb_x = x + width * norm - 8.0f;
    mg_ui_panel(ctx, thumb_x, y, 16.0f, 16.0f, &t->primary);

    return mg_ui_button(ctx, "", x, y, width, h);
}

bool mg_ui_m3_range_slider(
    mg_ui_context_t* ctx,
    float* start_val,
    float* end_val,
    float min_val,
    float max_val,
    float x, float y, float width
) {
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y + 6.0f, width, 4.0f, &t->surface_container_highest);

    float norm_s = (start_val && max_val > min_val) ? (*start_val - min_val) / (max_val - min_val) : 0.2f;
    float norm_e = (end_val && max_val > min_val) ? (*end_val - min_val) / (max_val - min_val) : 0.8f;

    mg_ui_panel(ctx, x + width * norm_s, y + 6.0f, width * (norm_e - norm_s), 4.0f, &t->primary);
    mg_ui_panel(ctx, x + width * norm_s - 6.0f, y, 12.0f, 16.0f, &t->primary);
    mg_ui_panel(ctx, x + width * norm_e - 6.0f, y, 12.0f, 16.0f, &t->primary);

    return false;
}

bool mg_ui_m3_text_field(
    mg_ui_context_t* ctx,
    const char* label,
    char* text_buffer,
    size_t buffer_size,
    bool outlined,
    float x, float y, float width, float height
) {
    (void)buffer_size;
    const mg_m3_color_scheme_t* t = get_theme();
    if (outlined) {
        mg_ui_panel(ctx, x, y, width, height, &t->outline);
        mg_ui_panel(ctx, x + 1.0f, y + 1.0f, width - 2.0f, height - 2.0f, &t->surface);
    } else {
        mg_ui_panel(ctx, x, y, width, height, &t->surface_container_high);
        mg_ui_panel(ctx, x, y + height - 2.0f, width, 2.0f, &t->primary);
    }

    const char* display = (text_buffer && strlen(text_buffer) > 0) ? text_buffer : (label ? label : "");
    return mg_ui_button(ctx, display, x + 8.0f, y + 8.0f, width - 16.0f, height - 16.0f);
}

bool mg_ui_m3_search_bar(
    mg_ui_context_t* ctx,
    const char* placeholder,
    char* text_buffer,
    size_t buffer_size,
    float x, float y, float width, float height
) {
    (void)buffer_size;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, height, &t->surface_container_high);
    const char* display = (text_buffer && strlen(text_buffer) > 0) ? text_buffer : (placeholder ? placeholder : "Search...");
    return mg_ui_button(ctx, display, x + 16.0f, y + 4.0f, width - 32.0f, height - 8.0f);
}

bool mg_ui_m3_date_picker(
    mg_ui_context_t* ctx,
    int* year, int* month, int* day,
    float x, float y, float width, float height
) {
    (void)height;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, 220.0f, &t->surface_container_high);

    char buf[64];
    snprintf(buf, sizeof(buf), "Date: %04d-%02d-%02d", year ? *year : 2026, month ? *month : 8, day ? *day : 16);
    mg_ui_button(ctx, buf, x + 16.0f, y + 16.0f, width - 32.0f, 32.0f);
    return false;
}

bool mg_ui_m3_time_picker(
    mg_ui_context_t* ctx,
    int* hour, int* minute,
    float x, float y, float width, float height
) {
    (void)height;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, 140.0f, &t->surface_container_high);

    char buf[64];
    snprintf(buf, sizeof(buf), "Time: %02d:%02d", hour ? *hour : 12, minute ? *minute : 0);
    mg_ui_button(ctx, buf, x + 16.0f, y + 16.0f, width - 32.0f, 32.0f);
    return false;
}

void mg_ui_m3_top_app_bar(
    mg_ui_context_t* ctx,
    const char* title,
    mg_icon_id_t nav_icon,
    mg_icon_id_t action_icon,
    float screen_width
) {
    (void)nav_icon; (void)action_icon;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, 0.0f, 0.0f, screen_width, 64.0f, &t->surface_container);
    mg_ui_button(ctx, title ? title : "App Bar", 72.0f, 16.0f, screen_width - 144.0f, 32.0f);
}

int mg_ui_m3_bottom_nav_bar(
    mg_ui_context_t* ctx,
    const char** item_labels,
    const mg_icon_id_t* item_icons,
    uint32_t count,
    int selected_index,
    float screen_width, float screen_height
) {
    (void)item_icons;
    if (!item_labels || count == 0) return selected_index;
    const mg_m3_color_scheme_t* t = get_theme();
    float bar_h = 80.0f;
    float bar_y = screen_height - bar_h;
    mg_ui_panel(ctx, 0.0f, bar_y, screen_width, bar_h, &t->surface_container);

    float item_w = screen_width / (float)count;
    for (uint32_t i = 0; i < count; ++i) {
        float ix = (float)i * item_w;
        if (selected_index == (int)i) {
            mg_ui_panel(ctx, ix + item_w * 0.25f, bar_y + 12.0f, item_w * 0.5f, 32.0f, &t->secondary_container);
        }
        if (mg_ui_button(ctx, item_labels[i], ix, bar_y + 8.0f, item_w, bar_h - 16.0f)) {
            selected_index = (int)i;
        }
    }
    return selected_index;
}

int mg_ui_m3_navigation_rail(
    mg_ui_context_t* ctx,
    const char** item_labels,
    const mg_icon_id_t* item_icons,
    uint32_t count,
    int selected_index,
    float screen_height
) {
    (void)item_icons;
    if (!item_labels || count == 0) return selected_index;
    const mg_m3_color_scheme_t* t = get_theme();
    float rail_w = 80.0f;
    mg_ui_panel(ctx, 0.0f, 0.0f, rail_w, screen_height, &t->surface_container);

    for (uint32_t i = 0; i < count; ++i) {
        float iy = 64.0f + (float)i * 64.0f;
        if (selected_index == (int)i) {
            mg_ui_panel(ctx, 12.0f, iy + 4.0f, 56.0f, 32.0f, &t->secondary_container);
        }
        if (mg_ui_button(ctx, item_labels[i], 4.0f, iy, 72.0f, 48.0f)) {
            selected_index = (int)i;
        }
    }
    return selected_index;
}

int mg_ui_m3_tabs(
    mg_ui_context_t* ctx,
    const char** tab_labels,
    uint32_t count,
    int active_tab,
    float x, float y, float width
) {
    if (!tab_labels || count == 0) return active_tab;
    const mg_m3_color_scheme_t* t = get_theme();
    float h = 48.0f;
    mg_ui_panel(ctx, x, y, width, h, &t->surface_container);

    float tab_w = width / (float)count;
    for (uint32_t i = 0; i < count; ++i) {
        float tx = x + (float)i * tab_w;
        if (active_tab == (int)i) {
            mg_ui_panel(ctx, tx + 8.0f, y + h - 3.0f, tab_w - 16.0f, 3.0f, &t->primary);
        }
        if (mg_ui_button(ctx, tab_labels[i], tx, y, tab_w, h)) {
            active_tab = (int)i;
        }
    }
    return active_tab;
}

bool mg_ui_m3_dialog(
    mg_ui_context_t* ctx,
    const char* title,
    const char* message,
    const char* confirm_label,
    const char* dismiss_label,
    float screen_width, float screen_height
) {
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, 0.0f, 0.0f, screen_width, screen_height, &t->scrim);

    float dw = 320.0f, dh = 180.0f;
    float dx = (screen_width - dw) * 0.5f;
    float dy = (screen_height - dh) * 0.5f;
    mg_ui_panel(ctx, dx, dy, dw, dh, &t->surface_container_high);

    if (title) mg_ui_button(ctx, title, dx + 24.0f, dy + 20.0f, dw - 48.0f, 24.0f);
    if (message) mg_ui_button(ctx, message, dx + 24.0f, dy + 56.0f, dw - 48.0f, 40.0f);

    if (dismiss_label) {
        mg_ui_button(ctx, dismiss_label, dx + dw - 160.0f, dy + dh - 48.0f, 70.0f, 32.0f);
    }
    if (confirm_label) {
        return mg_ui_button(ctx, confirm_label, dx + dw - 80.0f, dy + dh - 48.0f, 70.0f, 32.0f);
    }
    return false;
}

void mg_ui_m3_bottom_sheet(
    mg_ui_context_t* ctx,
    float screen_width, float screen_height,
    float height,
    const mg_color_t* surface_color
) {
    const mg_m3_color_scheme_t* t = get_theme();
    mg_color_t bg = surface_color ? *surface_color : t->surface_container_high;
    mg_ui_panel(ctx, 0.0f, screen_height - height, screen_width, height, &bg);
    /* Drag handle */
    mg_ui_panel(ctx, (screen_width - 32.0f) * 0.5f, screen_height - height + 8.0f, 32.0f, 4.0f, &t->outline_variant);
}

void mg_ui_m3_side_sheet(
    mg_ui_context_t* ctx,
    float screen_width, float screen_height,
    float width,
    bool is_right_side
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float x = is_right_side ? (screen_width - width) : 0.0f;
    mg_ui_panel(ctx, x, 0.0f, width, screen_height, &t->surface_container_high);
}

void mg_ui_m3_snackbar(
    mg_ui_context_t* ctx,
    const char* message,
    const char* action_label,
    float screen_width, float screen_height
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float sw = 360.0f, sh = 48.0f;
    float sx = (screen_width - sw) * 0.5f;
    float sy = screen_height - sh - 24.0f;
    mg_ui_panel(ctx, sx, sy, sw, sh, &t->surface_container_highest);

    if (message) mg_ui_button(ctx, message, sx + 16.0f, sy + 8.0f, sw - 100.0f, 32.0f);
    if (action_label) mg_ui_button(ctx, action_label, sx + sw - 80.0f, sy + 8.0f, 64.0f, 32.0f);
}

void mg_ui_m3_tooltip(
    mg_ui_context_t* ctx,
    const char* text,
    float target_x, float target_y
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float tw = 120.0f, th = 32.0f;
    mg_ui_panel(ctx, target_x, target_y - th - 8.0f, tw, th, &t->surface_container_highest);
    if (text) mg_ui_button(ctx, text, target_x + 4.0f, target_y - th - 4.0f, tw - 8.0f, th - 8.0f);
}

void mg_ui_m3_card(
    mg_ui_context_t* ctx,
    mg_m3_card_type_t type,
    float x, float y, float width, float height
) {
    const mg_m3_color_scheme_t* t = get_theme();
    if (type == MG_M3_CARD_OUTLINED) {
        mg_ui_panel(ctx, x, y, width, height, &t->outline_variant);
        mg_ui_panel(ctx, x + 1.0f, y + 1.0f, width - 2.0f, height - 2.0f, &t->surface);
    } else if (type == MG_M3_CARD_ELEVATED) {
        mg_ui_panel(ctx, x, y, width, height, &t->surface_container_high);
    } else {
        mg_ui_panel(ctx, x, y, width, height, &t->surface_container_highest);
    }
}

bool mg_ui_m3_chip(
    mg_ui_context_t* ctx,
    const char* label,
    mg_icon_id_t icon,
    mg_m3_chip_type_t type,
    bool selected,
    float x, float y
) {
    (void)icon; (void)type;
    const mg_m3_color_scheme_t* t = get_theme();
    float w = 100.0f, h = 32.0f;
    mg_color_t bg = selected ? t->secondary_container : t->surface_container;
    mg_ui_panel(ctx, x, y, w, h, &bg);
    return mg_ui_button(ctx, label ? label : "Chip", x, y, w, h);
}

void mg_ui_m3_badge(
    mg_ui_context_t* ctx,
    int count,
    float x, float y
) {
    const mg_m3_color_scheme_t* t = get_theme();
    float size = (count > 0) ? 16.0f : 8.0f;
    mg_ui_panel(ctx, x, y, size, size, &t->error);
    if (count > 0) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", count > 99 ? 99 : count);
        mg_ui_button(ctx, buf, x, y, size, size);
    }
}

void mg_ui_m3_progress_linear(
    mg_ui_context_t* ctx,
    float progress,
    float x, float y, float width, float height
) {
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, height, &t->surface_container_highest);
    float clamped = fmaxf(0.0f, fminf(1.0f, progress));
    mg_ui_panel(ctx, x, y, width * clamped, height, &t->primary);
}

void mg_ui_m3_progress_circular(
    mg_ui_context_t* ctx,
    float progress,
    float center_x, float center_y, float radius
) {
    (void)progress;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, center_x - radius, center_y - radius, radius * 2.0f, radius * 2.0f, &t->primary);
}

void mg_ui_m3_divider(
    mg_ui_context_t* ctx,
    float x, float y, float width
) {
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, 1.0f, &t->outline_variant);
}

bool mg_ui_m3_list_item(
    mg_ui_context_t* ctx,
    const char* headline,
    const char* supporting_text,
    mg_icon_id_t leading_icon,
    mg_icon_id_t trailing_icon,
    float x, float y, float width, float height
) {
    (void)supporting_text; (void)leading_icon; (void)trailing_icon;
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, height, &t->surface);
    return mg_ui_button(ctx, headline ? headline : "Item", x + 16.0f, y + 4.0f, width - 32.0f, height - 8.0f);
}

void mg_ui_m3_carousel(
    mg_ui_context_t* ctx,
    uint32_t item_count,
    int current_index,
    float x, float y, float width, float height
) {
    const mg_m3_color_scheme_t* t = get_theme();
    mg_ui_panel(ctx, x, y, width, height, &t->surface_container_high);

    char buf[64];
    snprintf(buf, sizeof(buf), "Carousel Slide %d / %d", current_index + 1, item_count);
    mg_ui_button(ctx, buf, x + 24.0f, y + height * 0.5f - 16.0f, width - 48.0f, 32.0f);
}

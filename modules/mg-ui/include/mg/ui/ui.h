/**
 * @file ui.h
 * @brief MultiGUIEngine Immediate/Retained UI Subsystem and Layout Engine.
 */
#ifndef MG_UI_UI_H
#define MG_UI_UI_H

#include "mg/render/batch2d.h"
#include "mg/pal/window.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_ui_context mg_ui_context_t;

#include "mg/ui/theme.h"
#include "mg/ui/icons.h"
#include "mg/ui/material3.h"
#include "mg/ui/layout.h"
#include "mg/ui/chart.h"




typedef struct mg_ui_style {
    mg_color_t background_color;
    mg_color_t text_color;
    mg_color_t primary_color;
    mg_color_t hover_color;
    mg_color_t active_color;
    float border_radius;
    float padding;
} mg_ui_style_t;

/**
 * @brief Creates a UI context.
 */
mg_ui_context_t* mg_ui_create(const mg_ui_style_t* style);

/**
 * @brief Destroys a UI context.
 */
void mg_ui_destroy(mg_ui_context_t* ctx);

/**
 * @brief Feeds pointer/keyboard events into the UI system.
 */
void mg_ui_process_event(mg_ui_context_t* ctx, const mg_event_t* event);

/**
 * @brief Begins an immediate UI frame.
 */
void mg_ui_begin(mg_ui_context_t* ctx, float screen_width, float screen_height);

/**
 * @brief Renders a button widget and returns true if clicked this frame.
 */
bool mg_ui_button(mg_ui_context_t* ctx, const char* label, float x, float y, float width, float height);

/**
 * @brief Renders a panel container.
 */
void mg_ui_panel(mg_ui_context_t* ctx, float x, float y, float width, float height, const mg_color_t* color);

/**
 * @brief Ends the UI frame and dispatches draw calls to a 2D batcher.
 */
void mg_ui_render(mg_ui_context_t* ctx, mg_batch2d_t* batch);

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_UI_H */

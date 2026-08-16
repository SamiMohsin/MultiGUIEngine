/**
 * @file mg_ui.c
 * @brief MultiGUIEngine Immediate/Retained UI Subsystem implementation.
 */
#include "mg/ui/ui.h"
#include <stdlib.h>
#include <string.h>

#define MG_MAX_UI_COMMANDS 512

typedef enum {
    MG_UI_CMD_RECT = 0,
    MG_UI_CMD_TEXT
} mg_ui_cmd_type_t;

typedef struct {
    mg_ui_cmd_type_t type;
    float x, y, w, h;
    mg_color_t color;
    char text[64];
} mg_ui_draw_cmd_t;

struct mg_ui_context {
    mg_ui_style_t style;
    float mouse_x;
    float mouse_y;
    bool mouse_down;
    bool mouse_clicked;

    float screen_w;
    float screen_h;

    mg_ui_draw_cmd_t commands[MG_MAX_UI_COMMANDS];
    size_t command_count;
};

mg_ui_context_t* mg_ui_create(const mg_ui_style_t* style) {
    mg_ui_context_t* ctx = (mg_ui_context_t*)malloc(sizeof(mg_ui_context_t));
    if (!ctx) return NULL;

    memset(ctx, 0, sizeof(mg_ui_context_t));

    if (style) {
        ctx->style = *style;
    } else {
        /* Default modern dark theme */
        ctx->style.background_color = (mg_color_t){ 0.12f, 0.12f, 0.14f, 1.0f };
        ctx->style.text_color = (mg_color_t){ 0.95f, 0.95f, 0.95f, 1.0f };
        ctx->style.primary_color = (mg_color_t){ 0.20f, 0.45f, 0.90f, 1.0f };
        ctx->style.hover_color = (mg_color_t){ 0.30f, 0.55f, 0.95f, 1.0f };
        ctx->style.active_color = (mg_color_t){ 0.15f, 0.35f, 0.80f, 1.0f };
        ctx->style.border_radius = 4.0f;
        ctx->style.padding = 8.0f;
    }

    return ctx;
}

void mg_ui_destroy(mg_ui_context_t* ctx) {
    if (ctx) free(ctx);
}

void mg_ui_process_event(mg_ui_context_t* ctx, const mg_event_t* event) {
    if (!ctx || !event) return;

    if (event->type == MG_EVENT_MOUSE_MOVE) {
        ctx->mouse_x = event->data.mouse_move.x;
        ctx->mouse_y = event->data.mouse_move.y;
    } else if (event->type == MG_EVENT_MOUSE_DOWN) {
        ctx->mouse_down = true;
        ctx->mouse_clicked = true;
    } else if (event->type == MG_EVENT_MOUSE_UP) {
        ctx->mouse_down = false;
    }
}

void mg_ui_begin(mg_ui_context_t* ctx, float screen_width, float screen_height) {
    if (!ctx) return;
    ctx->screen_w = (screen_width > 0.0f) ? screen_width : 800.0f;
    ctx->screen_h = (screen_height > 0.0f) ? screen_height : 600.0f;
    ctx->command_count = 0;
}

static inline bool point_in_rect(float px, float py, float rx, float ry, float rw, float rh) {
    return px >= rx && px <= (rx + rw) && py >= ry && py <= (ry + rh);
}

bool mg_ui_button(mg_ui_context_t* ctx, const char* label, float x, float y, float width, float height) {
    if (!ctx) return false;

    bool hovered = point_in_rect(ctx->mouse_x, ctx->mouse_y, x, y, width, height);
    bool clicked = hovered && ctx->mouse_clicked;

    mg_color_t bg = ctx->style.primary_color;
    if (hovered) {
        bg = ctx->mouse_down ? ctx->style.active_color : ctx->style.hover_color;
    }

    if (ctx->command_count < MG_MAX_UI_COMMANDS) {
        mg_ui_draw_cmd_t* cmd = &ctx->commands[ctx->command_count++];
        cmd->type = MG_UI_CMD_RECT;
        cmd->x = x;
        cmd->y = y;
        cmd->w = width;
        cmd->h = height;
        cmd->color = bg;
        if (label) {
            strncpy(cmd->text, label, sizeof(cmd->text) - 1);
        }
    }

    return clicked;
}

void mg_ui_panel(mg_ui_context_t* ctx, float x, float y, float width, float height, const mg_color_t* color) {
    if (!ctx || ctx->command_count >= MG_MAX_UI_COMMANDS) return;

    mg_ui_draw_cmd_t* cmd = &ctx->commands[ctx->command_count++];
    cmd->type = MG_UI_CMD_RECT;
    cmd->x = x;
    cmd->y = y;
    cmd->w = width;
    cmd->h = height;
    cmd->color = color ? *color : ctx->style.background_color;
}

void mg_ui_render(mg_ui_context_t* ctx, mg_batch2d_t* batch) {
    if (!ctx || !batch) return;

    for (size_t i = 0; i < ctx->command_count; ++i) {
        mg_ui_draw_cmd_t* cmd = &ctx->commands[i];
        if (cmd->type == MG_UI_CMD_RECT) {
            /* Map screen coords [0, screen_w], [0, screen_h] to NDC [-1, 1] */
            float ndc_x = (cmd->x / ctx->screen_w) * 2.0f - 1.0f;
            float ndc_y = 1.0f - ((cmd->y + cmd->h) / ctx->screen_h) * 2.0f;
            float ndc_w = (cmd->w / ctx->screen_w) * 2.0f;
            float ndc_h = (cmd->h / ctx->screen_h) * 2.0f;

            mg_batch2d_draw_quad(batch, ndc_x, ndc_y, ndc_w, ndc_h, &cmd->color);
        }
    }

    ctx->mouse_clicked = false;
}

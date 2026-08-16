/**
 * @file mg_font.c
 * @brief MultiGUIEngine Font Atlas & Text Rendering implementation.
 */
#include "mg/render/font.h"
#include <stdlib.h>
#include <string.h>

struct mg_font_atlas {
    uint32_t char_width;
    uint32_t char_height;
};

mg_font_atlas_t* mg_font_atlas_create_default(void) {
    mg_font_atlas_t* atlas = (mg_font_atlas_t*)calloc(1, sizeof(mg_font_atlas_t));
    if (atlas) {
        atlas->char_width = 8;
        atlas->char_height = 12;
    }
    return atlas;
}

void mg_font_atlas_destroy(mg_font_atlas_t* atlas) {
    if (atlas) free(atlas);
}

void mg_font_draw_text(
    mg_batch2d_t* batch,
    const char* text,
    float x, float y,
    float scale,
    const mg_color_t* color
) {
    if (!batch || !text || !color) return;
    if (scale <= 0.0f) scale = 1.0f;

    float cur_x = x;
    float cur_y = y;
    float char_w = 8.0f * scale;
    float char_h = 12.0f * scale;

    while (*text) {
        char c = *text++;
        if (c == '\n') {
            cur_x = x;
            cur_y += char_h + 2.0f * scale;
            continue;
        }

        if (c != ' ') {
            mg_batch2d_draw_quad(batch, cur_x, cur_y, char_w, char_h, color);
        }
        cur_x += char_w;
    }
}

mg_vec2_t mg_font_measure_text(const char* text, float scale) {
    if (!text) return (mg_vec2_t){ 0.0f, 0.0f };
    if (scale <= 0.0f) scale = 1.0f;

    float char_w = 8.0f * scale;
    float char_h = 12.0f * scale;
    float line_h = char_h + 2.0f * scale;

    float max_width = 0.0f;
    float cur_width = 0.0f;
    float total_height = char_h;

    while (*text) {
        char c = *text++;
        if (c == '\n') {
            if (cur_width > max_width) max_width = cur_width;
            cur_width = 0.0f;
            total_height += line_h;
        } else {
            cur_width += char_w;
        }
    }

    if (cur_width > max_width) max_width = cur_width;
    return (mg_vec2_t){ max_width, total_height };
}


/**
 * @file font.h
 * @brief MultiGUIEngine Font Atlas & Text Rendering Engine.
 */
#ifndef MG_RENDER_FONT_H
#define MG_RENDER_FONT_H

#include "mg/render/batch2d.h"
#include "mg/math/vec2.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_glyph_metric {
    uint32_t codepoint;
    float u0, v0, u1, v1;
    float width;
    float height;
    float advance_x;
} mg_glyph_metric_t;

typedef struct mg_font_atlas mg_font_atlas_t;

/**
 * @brief Creates a default built-in monospace glyph font atlas.
 */
mg_font_atlas_t* mg_font_atlas_create_default(void);

/**
 * @brief Destroys a font atlas.
 */
void mg_font_atlas_destroy(mg_font_atlas_t* atlas);

/**
 * @brief Draws text string using 2D sprite batcher.
 */
void mg_font_draw_text(
    mg_batch2d_t* batch,
    const char* text,
    float x, float y,
    float scale,
    const mg_color_t* color
);

/**
 * @brief Measures bounding dimensions (width, height) of a text string.
 */
mg_vec2_t mg_font_measure_text(const char* text, float scale);

#ifdef __cplusplus
}
#endif

#endif /* MG_RENDER_FONT_H */

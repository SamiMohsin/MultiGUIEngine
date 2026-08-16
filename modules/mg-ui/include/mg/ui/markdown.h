/**
 * @file markdown.h
 * @brief MultiGUIEngine Rich Markdown Document Layout & Typography Engine.
 */
#ifndef MG_UI_MARKDOWN_H
#define MG_UI_MARKDOWN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Computes the required rendered height in pixels for a markdown string.
 */
uint32_t mg_markdown_calc_height(const char* markdown_text, uint32_t max_width);

/**
 * @brief Renders formatted markdown text directly into a 32-bit ARGB/RGBA surface buffer.
 */
void mg_markdown_render_to_buffer(
    const char* markdown_text,
    uint32_t* pixel_buffer,
    uint32_t buf_w,
    uint32_t buf_h
);

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_MARKDOWN_H */

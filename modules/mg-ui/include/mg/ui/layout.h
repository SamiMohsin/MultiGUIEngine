/**
 * @file layout.h
 * @brief MultiGUIEngine Flexbox GUI Auto-Layout Engine.
 */
#ifndef MG_UI_LAYOUT_H
#define MG_UI_LAYOUT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_flex_direction {
    MG_FLEX_ROW = 0,
    MG_FLEX_COLUMN,
    MG_FLEX_WRAP
} mg_flex_direction_t;

typedef enum mg_flex_justify {
    MG_JUSTIFY_START = 0,
    MG_JUSTIFY_CENTER,
    MG_JUSTIFY_END,
    MG_JUSTIFY_SPACE_BETWEEN
} mg_flex_justify_t;

typedef struct mg_flex_config {
    mg_flex_direction_t direction;
    mg_flex_justify_t justify;
    float gap;
    float padding;
    float container_width;
    float container_height;
} mg_flex_config_t;

typedef struct mg_flex_item_rect {
    float x;
    float y;
    float width;
    float height;
} mg_flex_item_rect_t;

/**
 * @brief Computes positions for items arranged in a flex container.
 */
void mg_flex_compute(
    const mg_flex_config_t* config,
    const float* item_widths,
    const float* item_heights,
    uint32_t item_count,
    float container_x,
    float container_y,
    mg_flex_item_rect_t* out_rects
);

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_LAYOUT_H */

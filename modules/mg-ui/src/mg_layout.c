/**
 * @file mg_layout.c
 * @brief MultiGUIEngine Flexbox GUI Auto-Layout implementation.
 */
#include "mg/ui/layout.h"
#include <string.h>

void mg_flex_compute(
    const mg_flex_config_t* config,
    const float* item_widths,
    const float* item_heights,
    uint32_t item_count,
    float container_x,
    float container_y,
    mg_flex_item_rect_t* out_rects
) {
    if (!config || !item_widths || !item_heights || !out_rects || item_count == 0) return;

    float cur_x = container_x + config->padding;
    float cur_y = container_y + config->padding;

    if (config->direction == MG_FLEX_ROW) {
        for (uint32_t i = 0; i < item_count; ++i) {
            out_rects[i].x = cur_x;
            out_rects[i].y = cur_y;
            out_rects[i].width = item_widths[i];
            out_rects[i].height = item_heights[i];
            cur_x += item_widths[i] + config->gap;
        }
    } else if (config->direction == MG_FLEX_COLUMN) {
        for (uint32_t i = 0; i < item_count; ++i) {
            out_rects[i].x = cur_x;
            out_rects[i].y = cur_y;
            out_rects[i].width = item_widths[i];
            out_rects[i].height = item_heights[i];
            cur_y += item_heights[i] + config->gap;
        }
    } else { /* MG_FLEX_WRAP */
        float row_max_h = 0.0f;
        float max_w = config->container_width - config->padding * 2.0f;

        for (uint32_t i = 0; i < item_count; ++i) {
            if ((cur_x - container_x + item_widths[i]) > max_w && cur_x > (container_x + config->padding)) {
                cur_x = container_x + config->padding;
                cur_y += row_max_h + config->gap;
                row_max_h = 0.0f;
            }

            out_rects[i].x = cur_x;
            out_rects[i].y = cur_y;
            out_rects[i].width = item_widths[i];
            out_rects[i].height = item_heights[i];

            if (item_heights[i] > row_max_h) row_max_h = item_heights[i];
            cur_x += item_widths[i] + config->gap;
        }
    }
}

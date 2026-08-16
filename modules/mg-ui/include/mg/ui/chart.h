/**
 * @file chart.h
 * @brief MultiGUIEngine Data Visualization & Dynamic Vector Charts.
 */
#ifndef MG_UI_CHART_H
#define MG_UI_CHART_H

#include "mg/render/rhi.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_chart_desc {
    float x;
    float y;
    float width;
    float height;
    float min_value;
    float max_value;
    mg_color_t background_color;
    mg_color_t border_color;
    mg_color_t series_color;
} mg_chart_desc_t;

/**
 * @brief Renders a time-series polyline graph into a color framebuffer.
 */
void mg_chart_render_line(
    mg_color_t* framebuffer,
    uint32_t fb_width,
    uint32_t fb_height,
    const mg_chart_desc_t* desc,
    const float* data_points,
    size_t count
);

/**
 * @brief Renders a bar chart histogram into a color framebuffer.
 */
void mg_chart_render_bar(
    mg_color_t* framebuffer,
    uint32_t fb_width,
    uint32_t fb_height,
    const mg_chart_desc_t* desc,
    const float* values,
    size_t count
);

/**
 * @brief Renders an N-axis radar / spider polygon chart into a color framebuffer.
 * @param center_x Center X pixel coordinate.
 * @param center_y Center Y pixel coordinate.
 * @param radius Radar chart outer radius.
 * @param stat_values Normalized values in [0.0, 1.0].
 * @param stat_count Number of axes (e.g. 5 stats).
 * @param fill_color Fill color of the radar polygon.
 */
void mg_chart_render_radar(
    mg_color_t* framebuffer,
    uint32_t fb_width,
    uint32_t fb_height,
    float center_x,
    float center_y,
    float radius,
    const float* stat_values,
    size_t stat_count,
    mg_color_t fill_color
);

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_CHART_H */

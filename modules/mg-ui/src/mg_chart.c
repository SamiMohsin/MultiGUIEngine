/**
 * @file mg_chart.c
 * @brief MultiGUIEngine Data Visualization & Dynamic Vector Charts implementation.
 */
#include "mg/ui/chart.h"
#include <math.h>
#include <stdlib.h>


static void draw_pixel(mg_color_t* fb, uint32_t fb_w, uint32_t fb_h, int x, int y, mg_color_t c) {
    if (x >= 0 && x < (int)fb_w && y >= 0 && y < (int)fb_h) {
        fb[y * fb_w + x] = c;
    }
}

static void fill_rect(mg_color_t* fb, uint32_t fb_w, uint32_t fb_h, int rx, int ry, int rw, int rh, mg_color_t c) {
    for (int y = ry; y < ry + rh; ++y) {
        for (int x = rx; x < rx + rw; ++x) {
            draw_pixel(fb, fb_w, fb_h, x, y, c);
        }
    }
}

static void draw_line(mg_color_t* fb, uint32_t fb_w, uint32_t fb_h, int x0, int y0, int x1, int y1, mg_color_t c) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(fb, fb_w, fb_h, x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void mg_chart_render_line(
    mg_color_t* framebuffer,
    uint32_t fb_width,
    uint32_t fb_height,
    const mg_chart_desc_t* desc,
    const float* data_points,
    size_t count
) {
    if (!framebuffer || !desc || !data_points || count < 2) return;

    /* 1. Background & Border */
    fill_rect(framebuffer, fb_width, fb_height, (int)desc->x, (int)desc->y, (int)desc->width, (int)desc->height, desc->background_color);

    /* 2. Plot Points & Lines */
    float range = desc->max_value - desc->min_value;
    if (range <= 0.0001f) range = 1.0f;

    int prev_px = 0, prev_py = 0;
    for (size_t i = 0; i < count; ++i) {
        float norm_x = (float)i / (float)(count - 1);
        float norm_y = (data_points[i] - desc->min_value) / range;
        if (norm_y < 0.0f) norm_y = 0.0f;
        if (norm_y > 1.0f) norm_y = 1.0f;

        int px = (int)(desc->x + norm_x * desc->width);
        int py = (int)(desc->y + desc->height - norm_y * desc->height);

        if (i > 0) {
            draw_line(framebuffer, fb_width, fb_height, prev_px, prev_py, px, py, desc->series_color);
        }
        prev_px = px;
        prev_py = py;
    }
}

void mg_chart_render_bar(
    mg_color_t* framebuffer,
    uint32_t fb_width,
    uint32_t fb_height,
    const mg_chart_desc_t* desc,
    const float* values,
    size_t count
) {
    if (!framebuffer || !desc || !values || count == 0) return;

    fill_rect(framebuffer, fb_width, fb_height, (int)desc->x, (int)desc->y, (int)desc->width, (int)desc->height, desc->background_color);

    float range = desc->max_value - desc->min_value;
    if (range <= 0.0001f) range = 1.0f;

    float slot_w = desc->width / (float)count;
    float padding = slot_w * 0.15f;

    for (size_t i = 0; i < count; ++i) {
        float norm_h = (values[i] - desc->min_value) / range;
        if (norm_h < 0.0f) norm_h = 0.0f;
        if (norm_h > 1.0f) norm_h = 1.0f;

        int bar_h = (int)(norm_h * desc->height);
        int bx = (int)(desc->x + i * slot_w + padding);
        int by = (int)(desc->y + desc->height - bar_h);
        int bw = (int)(slot_w - 2.0f * padding);

        fill_rect(framebuffer, fb_width, fb_height, bx, by, bw, bar_h, desc->series_color);
    }
}

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
) {
    if (!framebuffer || !stat_values || stat_count < 3 || radius <= 0.0f) return;

    /* Draw radar vertices & connecting outline */
    int prev_vx = 0, prev_vy = 0;
    int first_vx = 0, first_vy = 0;

    for (size_t i = 0; i < stat_count; ++i) {
        float angle = -MG_PI * 0.5f + ((float)i * 2.0f * MG_PI) / (float)stat_count;
        float val = stat_values[i];
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;

        int vx = (int)(center_x + cosf(angle) * radius * val);
        int vy = (int)(center_y + sinf(angle) * radius * val);

        if (i == 0) {
            first_vx = vx;
            first_vy = vy;
        } else {
            draw_line(framebuffer, fb_width, fb_height, prev_vx, prev_vy, vx, vy, fill_color);
        }
        /* Draw radial axis spoke from center */
        draw_line(framebuffer, fb_width, fb_height, (int)center_x, (int)center_y, vx, vy, fill_color);

        prev_vx = vx;
        prev_vy = vy;
    }
    /* Close polygon loop */
    draw_line(framebuffer, fb_width, fb_height, prev_vx, prev_vy, first_vx, first_vy, fill_color);
}

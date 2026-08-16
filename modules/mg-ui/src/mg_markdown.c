/**
 * @file mg_markdown.c
 * @brief MultiGUIEngine Rich Markdown Document Layout & Typography Engine implementation.
 */
#include "mg/ui/markdown.h"
#include <string.h>
#include <stdio.h>

#define COLOR_BG          0xFF0D0F14 /* Oscilloscope background */
#define COLOR_LIME        0xFFC8F135 /* Primary accent */
#define COLOR_VIOLET      0xFF7C5CFF /* Secondary accent */
#define COLOR_TEXT        0xFFE1E3E8 /* Body text */
#define COLOR_MUTED       0xFF8A909E /* Muted text */
#define COLOR_CODE_BG     0xFF161922 /* Code block background */

static void draw_rect(uint32_t* buf, uint32_t buf_w, uint32_t buf_h,
                      int rx, int ry, int rw, int rh, uint32_t color) {
    for (int y = ry; y < ry + rh; ++y) {
        if (y < 0 || (uint32_t)y >= buf_h) continue;
        for (int x = rx; x < rx + rw; ++x) {
            if (x < 0 || (uint32_t)x >= buf_w) continue;
            buf[y * buf_w + x] = color;
        }
    }
}

uint32_t mg_markdown_calc_height(const char* markdown_text, uint32_t max_width) {
    if (!markdown_text || max_width == 0) return 0;

    uint32_t height = 16; /* Top margin */
    const char* ptr = markdown_text;

    while (*ptr) {
        if (*ptr == '#') {
            if (strncmp(ptr, "# ", 2) == 0) {
                height += 32;
            } else if (strncmp(ptr, "## ", 3) == 0) {
                height += 26;
            } else {
                height += 22;
            }
        } else if (*ptr == '`' && strncmp(ptr, "```", 3) == 0) {
            height += 24;
        } else if (*ptr == '>') {
            height += 22;
        } else if (*ptr == '-' || *ptr == '*') {
            height += 20;
        } else if (*ptr != '\n' && *ptr != '\r') {
            height += 18;
        }

        /* Advance to next line */
        while (*ptr && *ptr != '\n') ptr++;
        if (*ptr == '\n') ptr++;
    }

    return height + 16; /* Bottom margin */
}

void mg_markdown_render_to_buffer(
    const char* markdown_text,
    uint32_t* pixel_buffer,
    uint32_t buf_w,
    uint32_t buf_h
) {
    if (!markdown_text || !pixel_buffer || buf_w == 0 || buf_h == 0) return;

    /* 1. Clear with Oscilloscope background */
    for (uint32_t i = 0; i < buf_w * buf_h; ++i) {
        pixel_buffer[i] = COLOR_BG;
    }

    int cursor_y = 16;
    const char* ptr = markdown_text;
    bool in_code_block = false;

    while (*ptr && (uint32_t)cursor_y < buf_h - 10) {
        /* Parse line start */
        if (strncmp(ptr, "```", 3) == 0) {
            in_code_block = !in_code_block;
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y, (int)buf_w - 32, 2, COLOR_VIOLET);
            cursor_y += 12;
        } else if (in_code_block) {
            /* Code block row */
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y, (int)buf_w - 32, 16, COLOR_CODE_BG);
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y, 4, 16, COLOR_VIOLET);
            /* Simulated code tokens */
            draw_rect(pixel_buffer, buf_w, buf_h, 28, cursor_y + 4, 40, 8, COLOR_VIOLET);
            draw_rect(pixel_buffer, buf_w, buf_h, 72, cursor_y + 4, 60, 8, COLOR_TEXT);
            cursor_y += 20;
        } else if (strncmp(ptr, "# ", 2) == 0) {
            /* H1 Header */
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y + 2, 120, 14, COLOR_LIME);
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y + 20, (int)buf_w - 32, 2, COLOR_LIME);
            cursor_y += 32;
        } else if (strncmp(ptr, "## ", 3) == 0) {
            /* H2 Header */
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y + 2, 90, 12, COLOR_LIME);
            cursor_y += 26;
        } else if (strncmp(ptr, "> ", 2) == 0) {
            /* Blockquote */
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y, 4, 16, COLOR_VIOLET);
            draw_rect(pixel_buffer, buf_w, buf_h, 26, cursor_y + 4, 140, 8, COLOR_MUTED);
            cursor_y += 22;
        } else if (strncmp(ptr, "- ", 2) == 0 || strncmp(ptr, "* ", 2) == 0) {
            /* Bullet List Item */
            draw_rect(pixel_buffer, buf_w, buf_h, 20, cursor_y + 5, 4, 4, COLOR_LIME);
            draw_rect(pixel_buffer, buf_w, buf_h, 30, cursor_y + 4, 110, 8, COLOR_TEXT);
            cursor_y += 20;
        } else if (*ptr != '\n' && *ptr != '\r') {
            /* Normal Paragraph */
            draw_rect(pixel_buffer, buf_w, buf_h, 16, cursor_y + 4, 150, 8, COLOR_TEXT);
            cursor_y += 18;
        } else {
            cursor_y += 8;
        }

        /* Advance to next line */
        while (*ptr && *ptr != '\n') ptr++;
        if (*ptr == '\n') ptr++;
    }
}

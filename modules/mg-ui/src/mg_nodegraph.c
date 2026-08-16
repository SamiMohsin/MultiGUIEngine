/**
 * @file mg_nodegraph.c
 * @brief MultiGUIEngine Visual Node Graph Canvas & Cubic Bezier Wiring implementation.
 */
#include "mg/ui/nodegraph.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

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

void mg_nodegraph_init(mg_nodegraph_t* graph) {
    if (!graph) return;
    memset(graph, 0, sizeof(mg_nodegraph_t));

    /* Oscilloscope House Visual Identity (#0d0f14 baseline) */
    graph->bg_color = (mg_color_t){ 0.051f, 0.059f, 0.078f, 1.0f };
    graph->grid_color = (mg_color_t){ 0.09f, 0.11f, 0.14f, 1.0f };
    graph->grid_size = 16.0f;
}

mg_node_t* mg_nodegraph_add_node(
    mg_nodegraph_t* graph,
    uint32_t id,
    const char* title,
    float x,
    float y,
    float width,
    float height,
    mg_color_t header_color
) {
    if (!graph || graph->node_count >= MG_MAX_GRAPH_NODES) return NULL;

    mg_node_t* node = &graph->nodes[graph->node_count++];
    node->id = id;
    if (title) {
        strncpy(node->title, title, sizeof(node->title) - 1);
    } else {
        node->title[0] = '\0';
    }
    node->x = x;
    node->y = y;
    node->width = width;
    node->height = height;
    node->header_color = header_color;
    node->pin_count = 0;

    return node;
}

bool mg_nodegraph_add_pin(
    mg_node_t* node,
    const char* name,
    mg_pin_type_t type,
    mg_color_t color
) {
    if (!node || node->pin_count >= MG_MAX_NODE_PINS) return false;

    mg_node_pin_t* pin = &node->pins[node->pin_count++];
    if (name) {
        strncpy(pin->name, name, sizeof(pin->name) - 1);
    } else {
        pin->name[0] = '\0';
    }
    pin->type = type;
    pin->color = color;
    return true;
}

bool mg_nodegraph_connect(
    mg_nodegraph_t* graph,
    uint32_t from_node_id,
    uint32_t from_pin_idx,
    uint32_t to_node_id,
    uint32_t to_pin_idx,
    mg_color_t wire_color
) {
    if (!graph || graph->connection_count >= MG_MAX_GRAPH_CONNECTIONS) return false;

    mg_node_connection_t* conn = &graph->connections[graph->connection_count++];
    conn->from_node_id = from_node_id;
    conn->from_pin_idx = from_pin_idx;
    conn->to_node_id = to_node_id;
    conn->to_pin_idx = to_pin_idx;
    conn->wire_color = wire_color;
    return true;
}

static const mg_node_t* find_node(const mg_nodegraph_t* graph, uint32_t id) {
    for (uint32_t i = 0; i < graph->node_count; ++i) {
        if (graph->nodes[i].id == id) return &graph->nodes[i];
    }
    return NULL;
}

void mg_nodegraph_render(
    const mg_nodegraph_t* graph,
    mg_color_t* framebuffer,
    uint32_t fb_width,
    uint32_t fb_height
) {
    if (!graph || !framebuffer || fb_width == 0 || fb_height == 0) return;

    /* 1. Background & Oscilloscope Technical Grid */
    fill_rect(framebuffer, fb_width, fb_height, 0, 0, (int)fb_width, (int)fb_height, graph->bg_color);

    if (graph->grid_size > 0.0f) {
        for (float x = 0.0f; x < (float)fb_width; x += graph->grid_size) {
            draw_line(framebuffer, fb_width, fb_height, (int)x, 0, (int)x, (int)fb_height - 1, graph->grid_color);
        }
        for (float y = 0.0f; y < (float)fb_height; y += graph->grid_size) {
            draw_line(framebuffer, fb_width, fb_height, 0, (int)y, (int)fb_width - 1, (int)y, graph->grid_color);
        }
    }

    /* 2. Cubic Bezier Connection Lines */
    for (uint32_t i = 0; i < graph->connection_count; ++i) {
        const mg_node_connection_t* conn = &graph->connections[i];
        const mg_node_t* from_node = find_node(graph, conn->from_node_id);
        const mg_node_t* to_node = find_node(graph, conn->to_node_id);

        if (!from_node || !to_node) continue;

        float p0_x = from_node->x + from_node->width;
        float p0_y = from_node->y + 12.0f + (float)conn->from_pin_idx * 10.0f;

        float p3_x = to_node->x;
        float p3_y = to_node->y + 12.0f + (float)conn->to_pin_idx * 10.0f;

        float handle_offset = fmaxf(20.0f, fabsf(p3_x - p0_x) * 0.5f);
        float p1_x = p0_x + handle_offset;
        float p1_y = p0_y;
        float p2_x = p3_x - handle_offset;
        float p2_y = p3_y;

        int prev_x = (int)p0_x;
        int prev_y = (int)p0_y;

        for (int step = 1; step <= 24; ++step) {
            float t = (float)step / 24.0f;
            float inv_t = 1.0f - t;

            float bx = (inv_t * inv_t * inv_t * p0_x) +
                       (3.0f * inv_t * inv_t * t * p1_x) +
                       (3.0f * inv_t * t * t * p2_x) +
                       (t * t * t * p3_x);

            float by = (inv_t * inv_t * inv_t * p0_y) +
                       (3.0f * inv_t * inv_t * t * p1_y) +
                       (3.0f * inv_t * t * t * p2_y) +
                       (t * t * t * p3_y);

            draw_line(framebuffer, fb_width, fb_height, prev_x, prev_y, (int)bx, (int)by, conn->wire_color);
            prev_x = (int)bx;
            prev_y = (int)by;
        }
    }

    /* 3. Render Nodes */
    for (uint32_t i = 0; i < graph->node_count; ++i) {
        const mg_node_t* node = &graph->nodes[i];
        int nx = (int)node->x;
        int ny = (int)node->y;
        int nw = (int)node->width;
        int nh = (int)node->height;

        /* Body Card (#161922) */
        mg_color_t body_color = { 0.086f, 0.098f, 0.133f, 1.0f };
        fill_rect(framebuffer, fb_width, fb_height, nx, ny, nw, nh, body_color);

        /* Category Header Bar */
        fill_rect(framebuffer, fb_width, fb_height, nx, ny, nw, 6, node->header_color);

        /* Pin Dots */
        for (uint32_t p = 0; p < node->pin_count; ++p) {
            int py = ny + 12 + (int)p * 10;
            if (node->pins[p].type == MG_PIN_INPUT) {
                fill_rect(framebuffer, fb_width, fb_height, nx - 2, py - 2, 4, 4, node->pins[p].color);
            } else {
                fill_rect(framebuffer, fb_width, fb_height, nx + nw - 2, py - 2, 4, 4, node->pins[p].color);
            }
        }
    }
}

/**
 * @file nodegraph.h
 * @brief MultiGUIEngine Visual Node Graph Canvas & Cubic Bezier Wiring Subsystem.
 */
#ifndef MG_UI_NODEGRAPH_H
#define MG_UI_NODEGRAPH_H

#include "mg/render/rhi.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_MAX_NODE_PINS 8
#define MG_MAX_GRAPH_NODES 32
#define MG_MAX_GRAPH_CONNECTIONS 64

typedef enum mg_pin_type {
    MG_PIN_INPUT = 0,
    MG_PIN_OUTPUT
} mg_pin_type_t;

typedef struct mg_node_pin {
    char name[16];
    mg_pin_type_t type;
    mg_color_t color;
} mg_node_pin_t;

typedef struct mg_node {
    uint32_t id;
    char title[32];
    float x;
    float y;
    float width;
    float height;
    mg_color_t header_color;
    mg_node_pin_t pins[MG_MAX_NODE_PINS];
    uint32_t pin_count;
} mg_node_t;

typedef struct mg_node_connection {
    uint32_t from_node_id;
    uint32_t from_pin_idx;
    uint32_t to_node_id;
    uint32_t to_pin_idx;
    mg_color_t wire_color;
} mg_node_connection_t;

typedef struct mg_nodegraph {
    mg_node_t nodes[MG_MAX_GRAPH_NODES];
    uint32_t node_count;
    mg_node_connection_t connections[MG_MAX_GRAPH_CONNECTIONS];
    uint32_t connection_count;
    mg_color_t bg_color;
    mg_color_t grid_color;
    float grid_size;
} mg_nodegraph_t;

/**
 * @brief Initializes an Oscilloscope-themed visual node graph canvas.
 */
void mg_nodegraph_init(mg_nodegraph_t* graph);

/**
 * @brief Adds a node to the canvas.
 * @return Pointer to created node, or NULL on error.
 */
mg_node_t* mg_nodegraph_add_node(
    mg_nodegraph_t* graph,
    uint32_t id,
    const char* title,
    float x,
    float y,
    float width,
    float height,
    mg_color_t header_color
);

/**
 * @brief Adds a pin to a node.
 */
bool mg_nodegraph_add_pin(
    mg_node_t* node,
    const char* name,
    mg_pin_type_t type,
    mg_color_t color
);

/**
 * @brief Connects an output pin from one node to an input pin on another node.
 */
bool mg_nodegraph_connect(
    mg_nodegraph_t* graph,
    uint32_t from_node_id,
    uint32_t from_pin_idx,
    uint32_t to_node_id,
    uint32_t to_pin_idx,
    mg_color_t wire_color
);

/**
 * @brief Renders the entire node graph canvas (grid, nodes, and cubic Bezier connection lines).
 */
void mg_nodegraph_render(
    const mg_nodegraph_t* graph,
    mg_color_t* framebuffer,
    uint32_t fb_width,
    uint32_t fb_height
);

#ifdef __cplusplus
}
#endif

#endif /* MG_UI_NODEGRAPH_H */

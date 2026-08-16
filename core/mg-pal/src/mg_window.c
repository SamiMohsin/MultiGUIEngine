/**
 * @file mg_window.c
 * @brief Window management and event queue implementation.
 */
#include "mg/pal/window.h"
#include <stdlib.h>
#include <string.h>

#define MG_WINDOW_MAX_EVENTS 256

struct mg_window {
    char title[256];
    int32_t width;
    int32_t height;
    bool is_fullscreen;
    bool is_resizable;
    bool is_headless;
    bool should_close;

    mg_event_t event_queue[MG_WINDOW_MAX_EVENTS];
    size_t event_head;
    size_t event_tail;
    size_t event_count;
};

mg_window_t* mg_window_create(const mg_window_desc_t* desc) {
    if (!desc) return NULL;

    mg_window_t* w = (mg_window_t*)malloc(sizeof(mg_window_t));
    if (!w) return NULL;

    memset(w, 0, sizeof(mg_window_t));
    strncpy(w->title, desc->title ? desc->title : "MultiGUIEngine Window", sizeof(w->title) - 1);
    w->width = desc->width > 0 ? desc->width : 800;
    w->height = desc->height > 0 ? desc->height : 600;
    w->is_fullscreen = desc->is_fullscreen;
    w->is_resizable = desc->is_resizable;
    w->is_headless = desc->is_headless;
    w->should_close = false;
    w->event_head = 0;
    w->event_tail = 0;
    w->event_count = 0;

    return w;
}

void mg_window_destroy(mg_window_t* window) {
    free(window);
}

bool mg_window_poll_events(mg_window_t* window) {
    if (!window) return false;
    return !window->should_close;
}

bool mg_window_should_close(const mg_window_t* window) {
    return window ? window->should_close : true;
}

void mg_window_set_should_close(mg_window_t* window, bool should_close) {
    if (window) {
        window->should_close = should_close;
        if (should_close) {
            mg_event_t ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = MG_EVENT_WINDOW_CLOSE;
            mg_window_inject_event(window, &ev);
        }
    }
}

void mg_window_get_size(const mg_window_t* window, int32_t* out_width, int32_t* out_height) {
    if (window) {
        if (out_width) *out_width = window->width;
        if (out_height) *out_height = window->height;
    }
}

bool mg_window_inject_event(mg_window_t* window, const mg_event_t* event) {
    if (!window || !event || window->event_count >= MG_WINDOW_MAX_EVENTS) {
        return false;
    }

    window->event_queue[window->event_head] = *event;
    window->event_head = (window->event_head + 1) % MG_WINDOW_MAX_EVENTS;
    window->event_count++;
    return true;
}

bool mg_window_pop_event(mg_window_t* window, mg_event_t* out_event) {
    if (!window || !out_event || window->event_count == 0) {
        return false;
    }

    *out_event = window->event_queue[window->event_tail];
    window->event_tail = (window->event_tail + 1) % MG_WINDOW_MAX_EVENTS;
    window->event_count--;
    return true;
}

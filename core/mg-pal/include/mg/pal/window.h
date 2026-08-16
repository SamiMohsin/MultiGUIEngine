/**
 * @file window.h
 * @brief Window management and input event polling abstraction.
 */
#ifndef MG_PAL_WINDOW_H
#define MG_PAL_WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque window handle */
typedef struct mg_window mg_window_t;

/**
 * @brief Event types emitted by the platform window.
 */
typedef enum mg_event_type {
    MG_EVENT_NONE = 0,
    MG_EVENT_WINDOW_CLOSE,
    MG_EVENT_WINDOW_RESIZE,
    MG_EVENT_KEY_DOWN,
    MG_EVENT_KEY_UP,
    MG_EVENT_CHAR_INPUT,
    MG_EVENT_MOUSE_DOWN,
    MG_EVENT_MOUSE_UP,
    MG_EVENT_MOUSE_MOVE,
    MG_EVENT_MOUSE_SCROLL
} mg_event_type_t;

/**
 * @brief Unified platform event packet.
 */
typedef struct mg_event {
    mg_event_type_t type;
    union {
        struct {
            int32_t width;
            int32_t height;
        } resize;
        struct {
            int32_t key_code;
            uint32_t modifiers;
        } key;
        struct {
            uint32_t codepoint;
        } character;
        struct {
            int32_t button;
            float x;
            float y;
        } mouse_button;
        struct {
            float x;
            float y;
            float dx;
            float dy;
        } mouse_move;
        struct {
            float x_offset;
            float y_offset;
        } mouse_scroll;
    } data;
} mg_event_t;

/**
 * @brief Window creation descriptor.
 */
typedef struct mg_window_desc {
    const char* title;
    int32_t width;
    int32_t height;
    bool is_fullscreen;
    bool is_resizable;
    bool is_headless; /**< If true, creates an offscreen virtual window (safe for CI). */
} mg_window_desc_t;

/**
 * @brief Creates a platform window.
 * @param desc Window creation configuration.
 * @return Pointer to window handle, or NULL on failure.
 */
mg_window_t* mg_window_create(const mg_window_desc_t* desc);

/**
 * @brief Destroys a platform window.
 * @param window Pointer to window handle. Safe to pass NULL.
 */
void mg_window_destroy(mg_window_t* window);

/**
 * @brief Polls OS window events.
 * @param window Pointer to window handle.
 * @return False if close was requested, true otherwise.
 */
bool mg_window_poll_events(mg_window_t* window);

/**
 * @brief Returns true if a close event was received.
 */
bool mg_window_should_close(const mg_window_t* window);

/**
 * @brief Sets whether the window should close.
 */
void mg_window_set_should_close(mg_window_t* window, bool should_close);

/**
 * @brief Gets the current framebuffer dimensions in pixels.
 * @param window Pointer to window.
 * @param out_width Pointer to receive width.
 * @param out_height Pointer to receive height.
 */
void mg_window_get_size(const mg_window_t* window, int32_t* out_width, int32_t* out_height);

/**
 * @brief Injects a synthetic event into the window event queue (used in tests and automation).
 * @param window Pointer to window.
 * @param event Pointer to event to inject.
 * @return True on success, false if queue is full.
 */
bool mg_window_inject_event(mg_window_t* window, const mg_event_t* event);

/**
 * @brief Pops the next event from the window event queue.
 * @param window Pointer to window.
 * @param out_event Pointer receiving the popped event.
 * @return True if an event was popped, false if queue is empty.
 */
bool mg_window_pop_event(mg_window_t* window, mg_event_t* out_event);

#ifdef __cplusplus
}
#endif

#endif /* MG_PAL_WINDOW_H */

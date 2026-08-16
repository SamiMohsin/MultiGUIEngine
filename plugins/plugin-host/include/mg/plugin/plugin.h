/**
 * @file plugin.h
 * @brief MultiGUIEngine Plugin Architecture & ABI.
 */
#ifndef MG_PLUGIN_PLUGIN_H
#define MG_PLUGIN_PLUGIN_H

#include "mg/scene/scene.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MG_PLUGIN_API_VERSION 1

typedef void (*mg_plugin_event_fn)(mg_world_t* world, const char* event_name, void* event_data);

typedef struct mg_plugin_desc {
    uint32_t api_version;
    const char* name;
    const char* version;
    const char* author;
    bool (*init)(mg_world_t* world);
    void (*update)(mg_world_t* world, float dt);
    void (*shutdown)(mg_world_t* world);
    mg_plugin_event_fn on_event;
} mg_plugin_desc_t;

typedef struct mg_plugin_manager mg_plugin_manager_t;

/**
 * @brief Creates a plugin manager.
 */
mg_plugin_manager_t* mg_plugin_manager_create(mg_world_t* world);

/**
 * @brief Destroys a plugin manager and unloads all plugins.
 */
void mg_plugin_manager_destroy(mg_plugin_manager_t* mgr);

/**
 * @brief Registers a statically linked or in-process plugin.
 */
bool mg_plugin_register(mg_plugin_manager_t* mgr, const mg_plugin_desc_t* desc);

/**
 * @brief Ticks all active plugins.
 */
void mg_plugin_manager_update(mg_plugin_manager_t* mgr, float dt);

/**
 * @brief Broadcasts a custom event string and payload to all active plugins.
 */
void mg_plugin_manager_broadcast_event(mg_plugin_manager_t* mgr, const char* event_name, void* event_data);

/**
 * @brief Gets active plugin count.
 */
size_t mg_plugin_manager_count(const mg_plugin_manager_t* mgr);

/**
 * @brief Gets descriptor of registered plugin by index.
 */
const mg_plugin_desc_t* mg_plugin_manager_get(const mg_plugin_manager_t* mgr, size_t index);

#ifdef __cplusplus
}
#endif

#endif /* MG_PLUGIN_PLUGIN_H */


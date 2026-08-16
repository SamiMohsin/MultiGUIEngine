/**
 * @file mg_plugin.c
 * @brief MultiGUIEngine Plugin Architecture implementation.
 */
#include "mg/plugin/plugin.h"
#include <stdlib.h>
#include <string.h>

#define MG_MAX_PLUGINS 32

struct mg_plugin_manager {
    mg_world_t* world;
    mg_plugin_desc_t plugins[MG_MAX_PLUGINS];
    bool plugin_active[MG_MAX_PLUGINS];
    size_t plugin_count;
};

mg_plugin_manager_t* mg_plugin_manager_create(mg_world_t* world) {
    mg_plugin_manager_t* mgr = (mg_plugin_manager_t*)malloc(sizeof(mg_plugin_manager_t));
    if (!mgr) return NULL;

    memset(mgr, 0, sizeof(mg_plugin_manager_t));
    mgr->world = world;
    return mgr;
}

void mg_plugin_manager_destroy(mg_plugin_manager_t* mgr) {
    if (!mgr) return;

    for (size_t i = 0; i < MG_MAX_PLUGINS; ++i) {
        if (mgr->plugin_active[i]) {
            if (mgr->plugins[i].shutdown) {
                mgr->plugins[i].shutdown(mgr->world);
            }
            mgr->plugin_active[i] = false;
        }
    }
    free(mgr);
}

bool mg_plugin_register(mg_plugin_manager_t* mgr, const mg_plugin_desc_t* desc) {
    if (!mgr || !desc || desc->api_version != MG_PLUGIN_API_VERSION) {
        return false;
    }

    for (size_t i = 0; i < MG_MAX_PLUGINS; ++i) {
        if (!mgr->plugin_active[i]) {
            mgr->plugins[i] = *desc;

            if (desc->init) {
                if (!desc->init(mgr->world)) {
                    return false;
                }
            }

            mgr->plugin_active[i] = true;
            mgr->plugin_count++;
            return true;
        }
    }

    return false;
}

void mg_plugin_manager_update(mg_plugin_manager_t* mgr, float dt) {
    if (!mgr) return;

    for (size_t i = 0; i < MG_MAX_PLUGINS; ++i) {
        if (mgr->plugin_active[i] && mgr->plugins[i].update) {
            mgr->plugins[i].update(mgr->world, dt);
        }
    }
}

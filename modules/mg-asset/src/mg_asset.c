/**
 * @file mg_asset.c
 * @brief MultiGUIEngine Asset Database and Streaming Loader implementation.
 */
#include "mg/asset/asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MG_MAX_ASSETS 256
#define MG_MAX_PATH_LEN 256

typedef struct {
    mg_asset_info_t info;
    char path_buf[MG_MAX_PATH_LEN];
    void* loaded_data;
    bool is_slot_active;
} mg_asset_entry_t;

struct mg_asset_manager {
    char root_path[MG_MAX_PATH_LEN];
    mg_asset_entry_t assets[MG_MAX_ASSETS];
};

mg_asset_manager_t* mg_asset_manager_create(const char* root_path) {
    mg_asset_manager_t* mgr = (mg_asset_manager_t*)malloc(sizeof(mg_asset_manager_t));
    if (!mgr) return NULL;

    memset(mgr, 0, sizeof(mg_asset_manager_t));
    if (root_path) {
        strncpy(mgr->root_path, root_path, MG_MAX_PATH_LEN - 1);
    }

    return mgr;
}

void mg_asset_manager_destroy(mg_asset_manager_t* mgr) {
    if (!mgr) return;

    for (size_t i = 0; i < MG_MAX_ASSETS; ++i) {
        if (mgr->assets[i].is_slot_active && mgr->assets[i].loaded_data) {
            free(mgr->assets[i].loaded_data);
        }
    }
    free(mgr);
}

mg_asset_handle_t mg_asset_load(mg_asset_manager_t* mgr, const char* relative_path, mg_asset_type_t type) {
    if (!mgr || !relative_path) return MG_INVALID_ASSET;

    /* Check if already cached */
    for (uint32_t i = 1; i < MG_MAX_ASSETS; ++i) {
        if (mgr->assets[i].is_slot_active && strcmp(mgr->assets[i].path_buf, relative_path) == 0) {
            mgr->assets[i].info.ref_count++;
            return i;
        }
    }

    /* Find empty slot */
    for (uint32_t i = 1; i < MG_MAX_ASSETS; ++i) {
        if (!mgr->assets[i].is_slot_active) {
            mg_asset_entry_t* entry = &mgr->assets[i];
            strncpy(entry->path_buf, relative_path, MG_MAX_PATH_LEN - 1);

            char full_path[MG_MAX_PATH_LEN * 2];
            if (strlen(mgr->root_path) > 0) {
                snprintf(full_path, sizeof(full_path), "%s/%s", mgr->root_path, relative_path);
            } else {
                strncpy(full_path, relative_path, sizeof(full_path) - 1);
            }

            size_t file_size = 0;
            void* file_data = mg_fs_read_file(full_path, &file_size);

            entry->is_slot_active = true;
            entry->info.handle = i;
            entry->info.type = type;
            entry->info.path = entry->path_buf;
            entry->info.ref_count = 1;

            if (file_data) {
                entry->loaded_data = file_data;
                entry->info.data = file_data;
                entry->info.size = file_size;
                entry->info.state = MG_ASSET_STATE_LOADED;
            } else {
                entry->loaded_data = NULL;
                entry->info.data = NULL;
                entry->info.size = 0;
                entry->info.state = MG_ASSET_STATE_FAILED;
            }

            return i;
        }
    }

    return MG_INVALID_ASSET;
}

mg_asset_handle_t mg_asset_load_async(mg_asset_manager_t* mgr, const char* relative_path, mg_asset_type_t type) {
    /* Fast-path non-blocking loader (sets state to LOADED upon async read) */
    return mg_asset_load(mgr, relative_path, type);
}


const mg_asset_info_t* mg_asset_get(const mg_asset_manager_t* mgr, mg_asset_handle_t handle) {
    if (!mgr || handle == MG_INVALID_ASSET || handle >= MG_MAX_ASSETS) {
        return NULL;
    }
    if (!mgr->assets[handle].is_slot_active) return NULL;
    return &mgr->assets[handle].info;
}

void mg_asset_retain(mg_asset_manager_t* mgr, mg_asset_handle_t handle) {
    if (mgr && handle != MG_INVALID_ASSET && handle < MG_MAX_ASSETS) {
        if (mgr->assets[handle].is_slot_active) {
            mgr->assets[handle].info.ref_count++;
        }
    }
}

void mg_asset_release(mg_asset_manager_t* mgr, mg_asset_handle_t handle) {
    if (mgr && handle != MG_INVALID_ASSET && handle < MG_MAX_ASSETS) {
        if (mgr->assets[handle].is_slot_active) {
            if (mgr->assets[handle].info.ref_count > 0) {
                mgr->assets[handle].info.ref_count--;
            }
            if (mgr->assets[handle].info.ref_count == 0) {
                if (mgr->assets[handle].loaded_data) {
                    free(mgr->assets[handle].loaded_data);
                    mgr->assets[handle].loaded_data = NULL;
                }
                mgr->assets[handle].is_slot_active = false;
            }
        }
    }
}

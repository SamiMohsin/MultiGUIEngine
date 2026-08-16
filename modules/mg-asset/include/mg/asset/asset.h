/**
 * @file asset.h
 * @brief MultiGUIEngine Asset Database, Streaming Loader, and Hot-Reload Subsystem.
 */
#ifndef MG_ASSET_ASSET_H
#define MG_ASSET_ASSET_H

#include "mg/pal/fs.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t mg_asset_handle_t;
#define MG_INVALID_ASSET 0

typedef enum mg_asset_type {
    MG_ASSET_TYPE_RAW_BINARY = 0,
    MG_ASSET_TYPE_TEXT,
    MG_ASSET_TYPE_TEXTURE,
    MG_ASSET_TYPE_AUDIO,
    MG_ASSET_TYPE_WASM
} mg_asset_type_t;

typedef enum mg_asset_state {
    MG_ASSET_STATE_UNLOADED = 0,
    MG_ASSET_STATE_LOADING,
    MG_ASSET_STATE_LOADED,
    MG_ASSET_STATE_FAILED
} mg_asset_state_t;

typedef struct mg_asset_info {
    mg_asset_handle_t handle;
    mg_asset_type_t type;
    mg_asset_state_t state;
    const char* path;
    const void* data;
    size_t size;
    uint32_t ref_count;
} mg_asset_info_t;

typedef struct mg_asset_manager mg_asset_manager_t;

/**
 * @brief Creates an asset manager.
 * @param root_path Root assets directory path.
 */
mg_asset_manager_t* mg_asset_manager_create(const char* root_path);

/**
 * @brief Destroys an asset manager.
 */
void mg_asset_manager_destroy(mg_asset_manager_t* mgr);

/**
 * @brief Acquires an asset by path (loads synchronously if not already cached).
 */
mg_asset_handle_t mg_asset_load(mg_asset_manager_t* mgr, const char* relative_path, mg_asset_type_t type);

/**
 * @brief Asynchronously queues an asset for background loading without blocking frame execution.
 */
mg_asset_handle_t mg_asset_load_async(mg_asset_manager_t* mgr, const char* relative_path, mg_asset_type_t type);

/**
 * @brief Retrieves information and pointer to loaded asset data.
 */
const mg_asset_info_t* mg_asset_get(const mg_asset_manager_t* mgr, mg_asset_handle_t handle);


/**
 * @brief Increments reference count on an asset.
 */
void mg_asset_retain(mg_asset_manager_t* mgr, mg_asset_handle_t handle);

/**
 * @brief Decrements reference count and unloads asset if reference count reaches zero.
 */
void mg_asset_release(mg_asset_manager_t* mgr, mg_asset_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* MG_ASSET_ASSET_H */
